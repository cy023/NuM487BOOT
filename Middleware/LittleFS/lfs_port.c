/**
 * @file lfs_port.c
 * @author cy023
 * @date 2023.02.18
 * @brief 
 * 
 */

#include "lfs.h"
#include "w25q128jv.h"

/**
 * @brief   lfs porting Layer - "read" API.
 *          Read a region in a block. Negative error codes are propagated to the user.
 * @param  c        Configuration structure
 * @param  block    Block number
 * @param  off      Offset in the block
 * @param  buffer   Read data buffer
 * @param  size     Read bytes
 * @return
 */
static int lfs_deskio_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
    w25q128jv_read_sector((uint8_t *)buffer, block, off, size);
    return LFS_ERR_OK;
}

/**
 * @brief   lfs porting Layer - "write" API.
 *          Program a region in a block. The block must have previously
 *          been erased. Negative error codes are propagated to the user.
 *          May return LFS_ERR_CORRUPT if the block should be considered bad.
 * @param  c        Configuration structure
 * @param  block    Block number
 * @param  off      Offset in the block
 * @param  buffer   Write data buffer
 * @param  size     Write bytes
 * @return
 */
static int lfs_deskio_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
    w25q128jv_write_sector((uint8_t *)buffer, block, off, size);
    return LFS_ERR_OK;
}

/**
 * @brief   lfs porting Layer - "erase" API.
 *          Erase a block. A block must be erased before being programmed.
 *          The state of an erased block is undefined. Negative error codes
 *          are propagated to the user.
 *          May return LFS_ERR_CORRUPT if the block should be considered bad.
 * @param  c        Configuration structure
 * @param  block    Block number   
 * @return
 */
static int lfs_deskio_erase(const struct lfs_config *c, lfs_block_t block)
{
    w25q128jv_erase_sector(block);
    return LFS_ERR_OK;
}

/**
 * @brief   lfs porting Layer - "sync" API.
 *          Sync the state of the underlying block device. Negative error codes
 *          are propagated to the user.
 * @param  c        Configuration structure
 * @return int 
 */
static int lfs_deskio_sync(const struct lfs_config *c)
{
    return LFS_ERR_OK;
}

// Static allocated memory buffer
__attribute__((__aligned__(4))) static uint8_t read_buffer[16];
__attribute__((__aligned__(4))) static uint8_t prog_buffer[16];
__attribute__((__aligned__(4))) static uint8_t lookahead_buffer[16];

lfs_t lfs_w25q128jv;
lfs_file_t lfs_file_w25q128jv;

const struct lfs_config cfg = {
    // block device operations
    .read  = lfs_deskio_read,
    .prog  = lfs_deskio_prog,
    .erase = lfs_deskio_erase,
    .sync  = lfs_deskio_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,     // flash sector
    .block_count = 4096,    // flash sector count
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = 500,

    // Static allocated memory
    .read_buffer = read_buffer,
    .prog_buffer = prog_buffer,
    .lookahead_buffer = lookahead_buffer,
};
