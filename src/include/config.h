#ifndef CONFIG_H
#define CONFIG_H

namespace sjtu {

static constexpr int BUSTUB_PAGE_SIZE = 4096;                                        // size of a data page in byte
static constexpr int BUFFER_POOL_SIZE = 128;                                         // size of buffer pool
static constexpr int DEFAULT_DB_IO_SIZE = 16;                                        // starting size of file on disk
static constexpr int BUCKET_SIZE = 50;                                               // size of extendible hash bucket
static constexpr int LRUK_REPLACER_K = 10;                                           // backward k-distance for lru-k
static constexpr int MAX_SEAT_NUM = 100000;

using txn_id_t = int64_t;      // transaction id type

const txn_id_t TXN_START_ID = 1LL << 62;  // first txn id

}

#endif