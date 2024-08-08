// Contains definitions to help with configuring the MMIO registers of per-bank regulation unit
// Reference the BwRegulator Chisel code to see where these values come from

#define BASE 0x20000000ULL // Reg-map base address
#define MAX  0x20000800ULL // Reg-map max address
#define MAX_OFFSET 0x800ULL
#define NUM_REGS ( ( MAX - BASE ) / 8 ) // Assumes 8 byte fields, bit weird

#ifndef N_BANKS
#define N_BANKS 2
#endif

#define GLOBAL_EN 0
#define SETTINGS 1
#define PERIOD_LEN 2

#define MAX_READ(i) ( ( 24 + i * 8 ))
#define MAX_PUT(i)  ( ( 24 + 8 * m_nDomains + i * 8 ) )
#define MAX_WB(i)   ( ( 24 + 2 * 8 * m_nDomains + i * 8 ))

// Assuming you are reading 8 bytes at a time
#define CLIENT_EN_OFFSET ( ( 24 + 3 * 8 * m_nDomains ))

#define CLIENT_DOMAIN(i) ( ( 48 + 3 * 8 * m_nDomains + i * 8 ))

#define PERF_EN ( 48 + 3 * 8 * m_nDomains + m_nCPUs * 8 )

// Remove this?
#define PERF_PERIOD ( 56 + 3 * 8 * m_nDomains + m_nCPUs * 8 )

// i -> client, j -> bank
#define READ_CNTR(i, j) ( ( 8 * ( 8 + 3 * m_nDomains + m_nCPUs + i * m_nBanks + j ) ))
#define WRITE_CNTR(i, j) ( ( 8 * ( 8 + 3 * m_nDomains + N_CLIENTS + N_CLIENTS * N_BANKS + i * N_BANKS + j ) ))


