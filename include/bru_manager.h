#ifndef BRU_MANAGER_H
#define BRU_MANAGER_H
#include <vector>
#include <string>
#include <cassert>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "per-bank-map.h"


#define WRITE_BOOL(addr, value)(*(bool*)addr = value)
#define WRITE_UINT8(addr, value)(*(uint8_t*)addr = value)
#define WRITE_UINT16(addr, value)(*(uint16_t*)addr = value)
#define WRITE_UINT32(addr, value)(*(uint32_t*)addr = value)
#define WRITE_UINT64(addr, value)(*(uint64_t*)addr = value)

#define READ_BOOL(addr)(*(bool*)addr)
#define READ_UINT8(addr)(*(uint8_t*)addr)
#define READ_UINT16(addr)(*(uint16_t*)addr)
#define READ_UINT32(addr)(*(uint32_t*)addr)
#define READ_UINT64(addr)(*(uint64_t*)addr)



class BRUManager
{
public:
    BRUManager(uint32_t nDomains, uint32_t nBanks, uint32_t nCPUs, bool withMonitor=true, uint64_t regBaseAddress=0x20000000ULL);
    ~BRUManager();
    void SetRegulationPeriod(uint32_t periodLen);
    void EnableGlobal(bool enable);
    void EnableCountInstructionFetch(bool enable);
    void EnableWritebackThrottle(bool enable);
    void EnableCountPuts(bool enable);
    
    void SetDomainMaxReads(uint16_t domain, uint32_t nAccesses);
    uint32_t ReadDomainMaxReads(uint16_t domain) const;

    void SetDomainMaxWrites(uint16_t domain, uint32_t nWrites);
    uint32_t ReadDomainMaxWrites(uint16_t domain) const;

    void SetDomainMaxWritebacks(uint16_t domain, uint32_t nWritebacks);
    uint32_t ReadDomainMaxWritebacks(uint16_t domain);

    void EnableClientRegulation(uint32_t client_no, bool enable);
    bool ReadEnableClientRegulation(uint32_t client_no);

    void SetClientDomainID(uint32_t client_no, uint16_t domain);
    uint32_t ReadClientDomainID(uint32_t client_no);

    void EnablePerformanceCounters(bool enable);

    void SetPerformanceCountersPeriod(uint32_t period);
    uint32_t ReadPerformanceCountersPeriod();

    uint32_t ReadPerfCounter(uint16_t client_no, uint16_t bank);


private:
    int m_DevMemfd;
    uint64_t m_RegBase;
    uint32_t m_nDomains;
    uint32_t m_nBanks;
    uint32_t m_nCPUs;
    bool m_bWithMonitor;


    uint64_t m_EnableBRUGlobal;
    uint64_t m_Settings_CountInstrutionFetch;
    uint64_t m_Settings_EnableWritebackThrottle;
    uint64_t m_Settings_CountPuts;
    uint64_t m_RegulationPeriodLength;
    
};


#endif