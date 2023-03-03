//
// Created by Sebastian Yde Madsen on 01/03/2023.
//

#include "Utilities.hpp"


template <typename key_type, typename array_type>
class RedBlackTree {

public:
    // Attributes
    std::set<key_type> tree;

    // Standard un-parametrized C-tor.
    RedBlackTree() {};

    void insert(key_type& key) {
        this->tree.insert(key);
    }

    void insert_keys(array_type& keys){
        for(auto key: keys) insert(key);
    }

    bool holds(const key_type& key){
        if(this->tree.find(key) != this->tree.end())
        {
            return true;
        }
        return false;
    }

};