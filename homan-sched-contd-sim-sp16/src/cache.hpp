#ifndef CACHE_HPP
#define CACHE_HPP

#include <vector>
#include <iostream>

namespace MEMORY {

// FIFO CACHE
class CACHE {
protected:
  int capacity;
  std::vector<int> cache;

public:
  CACHE() : capacity(1024), cache(1024, -1){};
  CACHE(int c) : capacity(1024), cache(c, -1){};

  // Returns false for cache miss
  virtual bool get_page(int page) {
    for (int i = 0; i < capacity; i++) {
      if (cache[i] == page)
        return true;
      if (cache[i] == -1) {
        cache[i] = page;
        return false;
      }
    }

    cache.erase(cache.begin());
    cache.push_back(page);

    return false;
  }
};

// Most Recently Used Cache
class MRU_CACHE : public CACHE {
private:
  int mru; // index to MRU page

public:
  MRU_CACHE() : mru(0), CACHE(){};
  MRU_CACHE(int c) : mru(0), CACHE(c){};
  bool get_page(int page) {
    for (int i = 0; i < capacity; i++) {
      // Found Page
      if (cache[i] == page) {
        mru = i;
        return true;
      }
      // Found an empty slot
      if (cache[i] == -1) {
        mru = i; // Update MRU
        cache[i] = page;
        return false;
      }
    }

    // Replace MRU page
    cache[mru] = page;

    return false;
  }
};

// Least Recently Used Cache
class LRU_CACHE : public CACHE {
private:
  // Stores the page number
  // and an int representing
  // when it was last used
  // a.used > b.used means
  // b is LRU
  struct Page {
    Page() : page_num(-1), used(0){};
    int page_num;
    int used;
  };

  int lru;
  int mru; // Store the count of being used
  std::vector<Page> cache;

public:
  LRU_CACHE() : cache(1024), mru(0),lru(0),CACHE(){};
  LRU_CACHE(int c) : cache(c), mru(0),lru(0),CACHE(c){};
  bool get_page(int page) {
    for (int i = 0; i < capacity; i++) {
      // Found page and increment used
      if (cache[i].page_num == page) {
        cache[i].used = ++mru;
        return true;
      }
      // Found empty
      if (cache[i].page_num == -1) {
        cache[i].page_num = page;
        cache[i].used = ++mru;
        return false;
      } else {
        if (cache[i].used < cache[lru].used)
          lru = i;
      }
    }

    // Replace LRU
    cache[lru].page_num = page;
    cache[lru].used = ++mru;

    return false;
  }
};

class SC_CACHE : public CACHE {
private:
  // structure to store page number
  // and a flag for second chance
  struct Page {
    Page() : page_num(-1), second_chance(false){};
    int page_num;
    bool second_chance;
  };
  std::vector<Page> cache;

public:
  SC_CACHE() : cache(1024), CACHE(){};
  SC_CACHE(int c) : cache(c), CACHE(c){};
  bool get_page(int page) {
    for (int i = 0; i < capacity; i++) {
      // Found page and flip flag
      if (cache[i].page_num == page) {
        cache[i].second_chance = !cache[i].second_chance;
        return true;
      }
      // Found empty
      if (cache[i].page_num == -1) {
        cache[i].page_num = page;
        return false;
      }
    }

    // Replace first page without
    // second chance
    for (int i = 0; i < capacity; i++) {
      if (!cache[i].second_chance) {
        cache[i].page_num = page;
        return false;
      }
    }

    // SC devolves into FIFO
    // if cache is full of
    // pages with second chances
    cache.erase(cache.begin());
    Page p;
    p.page_num = page;
    cache.push_back(p);

    return false;
  }
};
}

#endif
