#pragma once
#include "../memoryriver/memoryriver.hpp"
#include "list.hpp"
#include "map.hpp"
#include "vector.hpp"
#include "String.hpp"


using namespace std;

// Serialization notes -------------------------------------------------------
// The B+ tree persists nodes on disk via MemoryRiver.  In order to support
// user-defined key/value types the library no longer relies on raw
// ``memcpy``; instead a simple traits-based serialization framework is
// provided in ``memoryriver.hpp``.  At compile time the tree asserts that
// both ``IndexType`` and ``ValueType`` are serializable.  There are three
// ways a type can be persisted:
//   * it is trivially copyable (POD-like) – the default path;
//   * it provides member functions ``serialize(ostream&)`` and
//     ``deserialize(istream&)``;
//   * or free functions ``serialize(ostream&, const T&)`` and
//     ``deserialize(istream&, T&)`` are available via ADL.
//
// The nested ``Key`` and ``Node`` types already implement the necessary
// members, so user code only needs to add serialization for their custom
// classes (for example, write a length-prefixed vector).  Types that admit
// a fixed-size serialized representation are required because the on-disk
// node size must remain constant; variable-length values must therefore be
// bounded or stored indirectly.
// ---------------------------------------------------------------------------

namespace sjtu {
template <typename IndexType, typename ValueType, int ORDER = 8>
class BPlusTree {
   public:
    // require that the template parameters can be persisted by our
    // serialization framework.  the static assertion text is intentionally
    // long so the compile error is helpful.
    static_assert(std::is_trivially_copyable_v<IndexType> ||
                      has_member_serializer<IndexType>::value ||
                      has_adl_serializer<IndexType>::value,
                  "IndexType must be trivially copyable or provide serialize/deserialize");
    static_assert(std::is_trivially_copyable_v<ValueType> ||
                      has_member_serializer<ValueType>::value ||
                      has_adl_serializer<ValueType>::value,
                  "ValueType must be trivially copyable or provide serialize/deserialize");
    struct Key {
        IndexType index;
        ValueType value;

        bool operator==(const Key& o) const {
            return index == o.index && value == o.value;
        }

        bool operator<(const Key& o) const {
            if (!(index == o.index)) return index < o.index;
            return value < o.value;
        }

        // serialization helpers required when ValueType/IndexType are
        // themselves non‑trivial.  MemoryRiver will dispatch to these via
        // the Serializer<T> trait defined in memoryriver.hpp.
    void serialize(std::ostream &os) const {
        Serializer<IndexType>::write(os, index);
        Serializer<ValueType>::write(os, value);
    }
        void deserialize(std::istream &is) {
            Serializer<IndexType>::read(is, index);
            Serializer<ValueType>::read(is, value);
        }
    };

   private:
    struct Node {
        bool is_leaf = false;
        int key_cnt = 0;
        int parent = -1;
        int next = -1;
        Key keys[ORDER];
        int child[ORDER + 1];

        Node() {
            // ensure deterministic contents so serialized blobs don't
            // contain uninitialised garbage; children set to -1 for safety.
            for (int i = 0; i < ORDER + 1; ++i)
                child[i] = -1;
        }

        // node serialization writes every field in deterministic order.
        // we intentionally avoid `memcpy` so that contained keys may themselves
        // be non‑trivial.
        void serialize(std::ostream &os) const {
            os.write(reinterpret_cast<const char *>(&is_leaf), sizeof(is_leaf));
            os.write(reinterpret_cast<const char *>(&key_cnt), sizeof(key_cnt));
            os.write(reinterpret_cast<const char *>(&parent), sizeof(parent));
            os.write(reinterpret_cast<const char *>(&next), sizeof(next));
            // write all key slots so that every node has identical length on disk
            for (int i = 0; i < ORDER; ++i)
                keys[i].serialize(os);
            // write all children entries (ORDER+1 ints)
            os.write(reinterpret_cast<const char *>(child),
                     sizeof(child[0]) * (ORDER + 1));
        }

        void deserialize(std::istream &is) {
            is.read(reinterpret_cast<char *>(&is_leaf), sizeof(is_leaf));
            is.read(reinterpret_cast<char *>(&key_cnt), sizeof(key_cnt));
            is.read(reinterpret_cast<char *>(&parent), sizeof(parent));
            is.read(reinterpret_cast<char *>(&next), sizeof(next));
            for (int i = 0; i < ORDER; ++i)
                keys[i].deserialize(is);
            is.read(reinterpret_cast<char *>(child),
                    sizeof(child[0]) * (ORDER + 1));
        }
    };

    MemoryRiver<Node, 2> river;

    struct CacheBlock {
        Node node;
        bool dirty = false;
        int pos;
    };

    static constexpr int CACHE_CAPACITY = 250;

    list<CacheBlock> cache_list;
    map<int, typename list<CacheBlock>::iterator> cache_map;

    Node node(int pos) {
        auto map_it = cache_map.find(pos);
        if (map_it != cache_map.end()) {
            auto old_list_it = map_it->second;
            Node current_node = old_list_it->node;
            bool current_dirty = old_list_it->dirty;
            cache_list.erase(old_list_it);
            cache_list.push_front({current_node, current_dirty, pos});
            cache_map[pos] = cache_list.begin();
            return current_node;
        }
        Node x;
        river.read(x, pos);
        if (cache_list.size() >= CACHE_CAPACITY) {
            CacheBlock& victim = cache_list.back();
            if (victim.dirty) {
                river.update(victim.node, victim.pos);
            }
            cache_map.erase(cache_map.find(victim.pos));
            cache_list.pop_back();
        }
        cache_list.push_front({x, false, pos});
        cache_map[pos] = cache_list.begin();
        return x;
    }

    void write_node(const Node& x, int pos) {
        auto map_it = cache_map.find(pos);
        if (map_it != cache_map.end()) {
            auto old_list_it = map_it->second;
            bool current_dirty = old_list_it->dirty;
            cache_list.erase(old_list_it);
            cache_list.push_front({x, true, pos});
            cache_map[pos] = cache_list.begin();
        } else {
            if (cache_list.size() >= CACHE_CAPACITY) {
                CacheBlock& victim = cache_list.back();
                if (victim.dirty) {
                    river.update(victim.node, victim.pos);
                }
                cache_map.erase(cache_map.find(victim.pos));
                cache_list.pop_back();
            }
            cache_list.push_front({x, true, pos});
            cache_map[pos] = cache_list.begin();
        }
    }

    Node add_to_cache(const Node& x, int pos) {
        if (cache_list.size() >= CACHE_CAPACITY) {
            CacheBlock& last = cache_list.back();
            if (last.dirty) {
                river.update(last.node, last.pos);
            }
            auto res = cache_map.find(last.pos);
            cache_map.erase(res);
            cache_list.pop_back();
        }
        cache_list.push_front({x, false, pos});
        cache_map[pos] = cache_list.begin();
        return x;
    }

    void flush_all() {
        for (auto& block : cache_list) {
            if (block.dirty) {
                river.update(block.node, block.pos);
                block.dirty = false;
            }
        }
    }

    int min_leaf_keys() const {
        return (ORDER + 1) / 2;
    }
    int min_internal_keys() const {
        return (ORDER - 1) / 2;
    }

    int root() {
        int r;
        river.get_info(r, 1);
        return r;
    }
    void set_root(int r) {
        river.write_info(r, 1);
    }

    int new_node(const Node& x) {
        int pos = river.write(x);
        add_to_cache(x, pos);
        int cnt;
        river.get_info(cnt, 2);
        river.write_info(cnt + 1, 2);
        return pos;
    }

    int find_leaf(const Key& key) {
        int u = root();
        while (true) {
            Node x = node(u);
            if (x.is_leaf) return u;
            int i = 0;
            while (i < x.key_cnt && !(key < x.keys[i])) i++;
            u = x.child[i];
        }
    }

    void insert_in_leaf(int u, const Key& key) {
        Node x = node(u);
        int i = x.key_cnt;
        while (i > 0 && key < x.keys[i - 1]) {
            x.keys[i] = x.keys[i - 1];
            i--;
        }
        x.keys[i] = key;
        x.key_cnt++;
        write_node(x, u);
        if (x.key_cnt == ORDER) {
            split_leaf(u);
        }
    }

    void split_leaf(int u) {
        Node x = node(u), y;
        y.is_leaf = true;
        y.parent = x.parent;

        int mid = ORDER / 2;
        y.key_cnt = x.key_cnt - mid;
        for (int i = 0; i < y.key_cnt; i++) {
            y.keys[i] = x.keys[mid + i];
        }

        x.key_cnt = mid;
        y.next = x.next;
        x.next = new_node(y);
        write_node(x, u);
        write_node(y, x.next);

        insert_in_parent(u, y.keys[0], x.next);
    }

    void insert_in_parent(int u, const Key& key, int v) {
        if (u == root()) {
            Node r;
            r.is_leaf = false;
            r.key_cnt = 1;
            r.keys[0] = key;
            r.child[0] = u;
            r.child[1] = v;
            int rp = new_node(r);

            Node cu = node(u);
            cu.parent = rp;
            write_node(cu, u);
            Node cv = node(v);
            cv.parent = rp;
            write_node(cv, v);

            set_root(rp);
            return;
        }

        Node cur = node(u);
        int p = cur.parent;
        Node par = node(p);

        int i = par.key_cnt;
        while (i > 0 && key < par.keys[i - 1]) {
            par.keys[i] = par.keys[i - 1];
            par.child[i + 1] = par.child[i];
            i--;
        }
        par.keys[i] = key;
        par.child[i + 1] = v;
        par.key_cnt++;

        Node cv = node(v);
        cv.parent = p;
        write_node(cv, v);
        write_node(par, p);

        if (par.key_cnt == ORDER) split_internal(p);
    }

    void split_internal(int u) {
        Node x = node(u), y;
        y.is_leaf = false;
        y.parent = x.parent;

        int mid = ORDER / 2;
        Key up = x.keys[mid];

        y.key_cnt = x.key_cnt - mid - 1;
        for (int i = 0; i < y.key_cnt; i++) {
            y.keys[i] = x.keys[mid + 1 + i];
        }

        for (int i = 0; i <= y.key_cnt; i++) {
            y.child[i] = x.child[mid + 1 + i];
        }

        x.key_cnt = mid;
        int v = new_node(y);
        for (int i = 0; i <= y.key_cnt; i++) {
            Node c = node(y.child[i]);
            c.parent = v;
            write_node(c, y.child[i]);
        }

        write_node(x, u);
        write_node(y, v);
        insert_in_parent(u, up, v);
    }

    void fix_leaf(int u) {
        Node cur = node(u);
        int p = cur.parent;
        Node par = node(p);
        int idx = 0;
        while (par.child[idx] != u) idx++;

        if (idx > 0) {
            int l = par.child[idx - 1];
            Node left = node(l);
            if (left.key_cnt > min_leaf_keys()) {
                for (int i = cur.key_cnt; i > 0; i--)
                    cur.keys[i] = cur.keys[i - 1];
                cur.keys[0] = left.keys[left.key_cnt - 1];
                cur.key_cnt++;
                left.key_cnt--;
                par.keys[idx - 1] = cur.keys[0];
                write_node(left, l);
                write_node(cur, u);
                write_node(par, p);
                return;
            }
        }

        if (idx + 1 <= par.key_cnt) {
            int r = par.child[idx + 1];
            Node right = node(r);
            if (right.key_cnt > min_leaf_keys()) {
                cur.keys[cur.key_cnt++] = right.keys[0];
                for (int i = 0; i + 1 < right.key_cnt; i++)
                    right.keys[i] = right.keys[i + 1];
                right.key_cnt--;
                par.keys[idx] = right.keys[0];
                write_node(right, r);
                write_node(cur, u);
                write_node(par, p);
                return;
            }
        }

        if (idx > 0)
            merge_leaf(par.child[idx - 1], u, idx - 1);
        else
            merge_leaf(u, par.child[idx + 1], idx);
    }

    void merge_leaf(int l, int r, int sep) {
        Node left = node(l);
        Node right = node(r);
        Node par = node(left.parent);

        for (int i = 0; i < right.key_cnt; ++i)
            left.keys[left.key_cnt + i] = right.keys[i];
        left.key_cnt += right.key_cnt;
        left.next = right.next;
        for (int i = sep; i + 1 < par.key_cnt; ++i) {
            par.keys[i] = par.keys[i + 1];
            par.child[i + 1] = par.child[i + 2];
        }
        par.key_cnt--;

        write_node(left, l);
        write_node(par, left.parent);

        if (par.parent != -1 && par.key_cnt < min_internal_keys())
            fix_internal(left.parent);

        if (par.key_cnt == 0 && left.parent == root()) {
            set_root(l);
            left.parent = -1;
            write_node(left, l);
        }
    }

    void fix_internal(int u) {
        Node x = node(u);
        if (x.parent == -1 || x.key_cnt >= min_internal_keys()) return;

        Node par = node(x.parent);
        int idx = 0;
        while (par.child[idx] != u) idx++;

        if (idx > 0) {
            int l = par.child[idx - 1];
            Node left = node(l);
            if (left.key_cnt > min_internal_keys()) {
                for (int i = x.key_cnt; i > 0; i--) x.keys[i] = x.keys[i - 1];
                for (int i = x.key_cnt + 1; i > 0; i--)
                    x.child[i] = x.child[i - 1];
                x.keys[0] = par.keys[idx - 1];
                x.child[0] = left.child[left.key_cnt];
                Node c = node(x.child[0]);
                c.parent = u;
                write_node(c, x.child[0]);
                par.keys[idx - 1] = left.keys[left.key_cnt - 1];
                x.key_cnt++;
                left.key_cnt--;
                write_node(left, l);
                write_node(x, u);
                write_node(par, x.parent);
                return;
            }
        }

        if (idx < par.key_cnt) {
            int r = par.child[idx + 1];
            Node right = node(r);
            if (right.key_cnt > min_internal_keys()) {
                x.keys[x.key_cnt] = par.keys[idx];
                x.child[x.key_cnt + 1] = right.child[0];
                Node c = node(x.child[x.key_cnt + 1]);
                c.parent = u;
                write_node(c, x.child[x.key_cnt + 1]);
                par.keys[idx] = right.keys[0];
                for (int i = 0; i + 1 < right.key_cnt; i++)
                    right.keys[i] = right.keys[i + 1];
                for (int i = 0; i + 1 <= right.key_cnt; i++)
                    right.child[i] = right.child[i + 1];
                x.key_cnt++;
                right.key_cnt--;
                write_node(right, r);
                write_node(x, u);
                write_node(par, x.parent);
                return;
            }
        }
        if (idx > 0)
            merge_internal(par.child[idx - 1], u, idx - 1);
        else
            merge_internal(u, par.child[idx + 1], idx);
    }

    void merge_internal(int l, int r, int sep) {
        Node left = node(l);
        Node right = node(r);
        Node par = node(left.parent);

        left.keys[left.key_cnt++] = par.keys[sep];
        for (int i = 0; i < right.key_cnt; i++) {
            left.keys[left.key_cnt + i] = right.keys[i];
        }

        for (int i = 0; i <= right.key_cnt; i++) {
            left.child[left.key_cnt + i] = right.child[i];
            Node c = node(right.child[i]);
            c.parent = l;
            write_node(c, right.child[i]);
        }

        left.key_cnt += right.key_cnt;

        for (int i = sep; i + 1 < par.key_cnt; i++) {
            par.keys[i] = par.keys[i + 1];
            par.child[i + 1] = par.child[i + 2];
        }
        par.key_cnt--;

        write_node(left, l);
        write_node(par, left.parent);

        if (par.parent == -1 && par.key_cnt == 0) {
            set_root(l);
            left.parent = -1;
            write_node(left, l);
            return;
        }

        if (par.parent != -1 && par.key_cnt < min_internal_keys()) {
            fix_internal(left.parent);
        }
    }

   public:
    BPlusTree(string fn = "bpt.db") {
        river.initialise(fn);
        int cnt;
        river.get_info(cnt, 2);
        if (cnt == 0) {
            Node r;
            r.is_leaf = true;
            int rp = river.write(r);
            river.write_info(rp, 1);
            river.write_info(1, 2);
        }
    }

    ~BPlusTree() {
        flush_all();
    }

    void insert(const IndexType& idx, const ValueType& val) {
        Key key{};
        key.index = idx;
        key.value = val;
        int u = find_leaf(key);
        Node x = node(u);
        for (int i = 0; i < x.key_cnt; i++)
            if (x.keys[i] == key) return;
        insert_in_leaf(u, key);
    }

    void erase(const IndexType& idx, const ValueType& val) {
        Key key{};
        key.index = idx;
        key.value = val;
        int u = find_leaf(key);
        Node x = node(u);
        int pos = -1;
        for (int i = 0; i < x.key_cnt; i++)
            if (x.keys[i] == key) pos = i;
        if (pos == -1) return;
        for (int i = pos; i + 1 < x.key_cnt; i++) x.keys[i] = x.keys[i + 1];
        x.key_cnt--;
        write_node(x, u);
        if (u != root() && x.key_cnt < min_leaf_keys()) fix_leaf(u);
    }

    vector<Key> find(const IndexType& idx) {
        Key low{};
        low.index = idx;
        if constexpr (std::is_same_v<ValueType, String>) {
            low.value = String::min_value();
        } else {
            low.value = ValueType{};
        }
        int u = find_leaf(low);
        vector<Key> result;
        while (u != -1) {
            Node x = node(u);
            bool found = false;
            for (int i = 0; i < x.key_cnt; i++) {
                if (x.keys[i].index == low.index) {
                    result.push_back(x.keys[i]);
                    found = true;
                } else if (x.keys[i].index > low.index) {
                    return result;
                }
            }
            u = x.next;
        }
        return result;
    }
    void clean_up() {
        cache_list.clear();
        cache_map.clear();

        river.clean_up();
        Node r;
        r.is_leaf = true;
        r.parent = -1;
        r.key_cnt = 0;

        int rp = river.write(r);
        river.write_info(rp, 1);
        river.write_info(1, 2);
        add_to_cache(r, rp);
    }
};

}  // namespace sjtu