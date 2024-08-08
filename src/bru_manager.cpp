#include "bru_manager.h"

BRUManager::BRUManager(uint32_t nDomains, uint32_t nBanks, uint32_t nCPUs, bool withMonitor, uint64_t regBaseAddress) : \
    m_nDomains(nDomains), m_nBanks(nBanks), m_nCPUs(nCPUs), m_bWithMonitor(withMonitor)
{
    m_DevMemfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (m_DevMemfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Map physical memory into the process's address space
    m_RegBase = (uint64_t)mmap(NULL, MAX_OFFSET, PROT_READ | PROT_WRITE, MAP_SHARED, m_DevMemfd, regBaseAddress);
    if (m_RegBase == (uint64_t)MAP_FAILED) {
        perror("mmap");
        close(m_DevMemfd);
        exit(EXIT_FAILURE);
    }



    m_EnableBRUGlobal = m_RegBase;
    m_Settings_CountInstrutionFetch = m_RegBase + 0x8;
    m_Settings_EnableWritebackThrottle = m_Settings_CountInstrutionFetch + 0x1;
    m_Settings_CountPuts = m_Settings_EnableWritebackThrottle + 0x1;
    m_RegulationPeriodLength = m_RegBase + 0x10;
}

BRUManager::BRUManager(uint64_t regBaseAddress)
{
    m_DevMemfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (m_DevMemfd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Map physical memory into the process's address space
    m_RegBase = (uint64_t)mmap(NULL, MAX_OFFSET, PROT_READ | PROT_WRITE, MAP_SHARED, m_DevMemfd, regBaseAddress);
    if (m_RegBase == (uint64_t)MAP_FAILED) {
        perror("mmap");
        close(m_DevMemfd);
        exit(EXIT_FAILURE);
    }

    m_EnableBRUGlobal = m_RegBase;
    m_Settings_CountInstrutionFetch = m_RegBase + 0x8;
    m_Settings_EnableWritebackThrottle = m_Settings_CountInstrutionFetch + 0x1;
    m_Settings_CountPuts = m_Settings_EnableWritebackThrottle + 0x1;
    m_RegulationPeriodLength = m_RegBase + 0x10;


    uint64_t MAX_ADDR = m_RegBase + MAX_OFFSET;
    m_nBanks = READ_UINT32(MAX_ADDR-0x4);
    m_nDomains = READ_UINT32(MAX_ADDR-0X8);
    m_nCPUs = READ_UINT32(MAX_ADDR-0XC);
    m_bWithMonitor = READ_BOOL(MAX_ADDR-0X10);
}

BRUManager::~BRUManager()
{
    munmap((void*)m_RegBase, 0x800);
    close(m_DevMemfd);
}

void BRUManager::SetRegulationPeriod(uint32_t periodLen) 
{
   WRITE_UINT64(m_RegulationPeriodLength, periodLen);
}

void BRUManager::EnableGlobal(bool enable) 
{
    WRITE_BOOL(m_EnableBRUGlobal, enable);
}

void BRUManager::EnableCountInstructionFetch(bool enable) 
{
    WRITE_BOOL(m_Settings_CountInstrutionFetch, enable);
}

void BRUManager::EnableWritebackThrottle(bool enable) 
{
    WRITE_BOOL(m_Settings_EnableWritebackThrottle, enable);
}

void BRUManager::EnableCountPuts(bool enable) 
{
    WRITE_BOOL(m_Settings_CountPuts, enable);
}

void BRUManager::SetDomainMaxReads(uint16_t domain, uint32_t nAccesses) 
{
    assert(domain < m_nDomains);
    uint64_t domainAccessCounterAddr = m_RegBase + MAX_READ(domain);
    printf("BRUManager::SetDomainMaxReads() to %ld at 0x%xx\n", nAccesses, MAX_READ(domain));
    WRITE_UINT32(domainAccessCounterAddr, nAccesses);
}

uint32_t BRUManager::ReadDomainMaxReads(uint16_t domain) const {
    assert(domain < m_nDomains);
    printf("Offset 0x%x\n", MAX_READ(domain));
    uint64_t domainAccessCounterAddr = m_RegBase + MAX_READ(domain);
    return READ_UINT32(domainAccessCounterAddr);
}

void BRUManager::SetDomainMaxWrites(uint16_t domain, uint32_t nWrites) 
{
    assert(domain < m_nDomains);
    printf("Offset 0x%x\n", MAX_PUT(domain));
    uint64_t domainWriteCounterAddr = m_RegBase + MAX_PUT(domain);
    WRITE_UINT32(domainWriteCounterAddr, nWrites);
}

uint32_t BRUManager::ReadDomainMaxWrites(uint16_t domain) const 
{
    assert(domain < m_nDomains);
    printf("Offset 0x%x\n", MAX_PUT(domain));
    uint64_t domainWriteCounterAddr = m_RegBase + MAX_PUT(domain);
    return READ_UINT32(domainWriteCounterAddr);
}

void BRUManager::SetDomainMaxWritebacks(uint16_t domain, uint32_t nWritebacks) 
{
    assert(domain < m_nDomains);
    printf("Offset 0x%x\n", MAX_WB(domain));
    uint64_t domainWritebackCounterAddr = m_RegBase + MAX_WB(domain);
    WRITE_UINT32(domainWritebackCounterAddr, nWritebacks);
}

uint32_t BRUManager::ReadDomainMaxWritebacks(uint16_t domain) 
{
    assert(domain < m_nDomains);
    printf("Offset 0x%x\n", MAX_WB(domain));
    uint64_t domainWritebackCounterAddr = m_RegBase + MAX_WB(domain);
    return READ_UINT32(domainWritebackCounterAddr);
}

void BRUManager::EnableClientRegulation(uint32_t client_no, bool enable)
{
    assert(client_no < m_nCPUs);


    printf("Offset 0x%x\n enable %d", CLIENT_EN_OFFSET, enable);
    uint64_t clientEnableRegulationAddr = m_RegBase + CLIENT_EN_OFFSET;
    uint64_t value = READ_UINT64(clientEnableRegulationAddr);
    printf("value before 0x%x\n", value);
    value &= ~(1 << client_no); // unset client no
    value |= (enable << client_no); // set bit to enable
    printf("after 0x%x\n", value); 
    WRITE_UINT64(clientEnableRegulationAddr, value);
}

bool BRUManager::ReadEnableClientRegulation(uint32_t client_no) {
    assert(client_no < m_nCPUs);
    uint64_t clientEnableRegulationAddr = m_RegBase + CLIENT_EN_OFFSET;
    uint64_t value = READ_UINT64(clientEnableRegulationAddr);
    
    return (bool)((value >> client_no) & 0x1);
}

void BRUManager::SetClientDomainID(uint32_t client_no, uint16_t domain)
{   
    assert(client_no < m_nCPUs);
    uint64_t clientDomainIDAddr = m_RegBase + CLIENT_DOMAIN(client_no);
    printf("Offset 0x%x\n", CLIENT_DOMAIN(client_no));
    /*
        These registers are of size log2Ceil(nDomains) but they are spaced
        8 bytes apart so this should be fine
    */
    WRITE_UINT16(clientDomainIDAddr, domain);
}

uint32_t BRUManager::ReadClientDomainID(uint32_t client_no)
{
    assert(client_no < m_nCPUs);
    uint64_t clientDomainIDAddr = m_RegBase + CLIENT_DOMAIN(client_no);
    printf("Offset 0x%x\n", CLIENT_DOMAIN(client_no));
    /*
        These registers are of size log2Ceil(nDomains) but they are spaced
        8 bytes apart so this should be fine
    */
    return READ_UINT16(client_no);
}

void BRUManager::EnablePerformanceCounters(bool enable)
{
    uint64_t enablePerfCounterAddr = m_RegBase + PERF_EN;
    WRITE_BOOL(enablePerfCounterAddr, enable);
}

void BRUManager::SetPerformanceCountersPeriod(uint32_t period)
{
    uint64_t perfCounterPeriodAddr = m_RegBase + PERF_PERIOD;
    WRITE_UINT32(perfCounterPeriodAddr, period);
}

uint32_t BRUManager::ReadPerformanceCountersPeriod()
{
    uint64_t perfCounterPeriodAddr = m_RegBase + PERF_PERIOD;
    return READ_UINT32(perfCounterPeriodAddr);
}

uint32_t BRUManager::ReadPerfCounter(uint16_t client_no, uint16_t bank)
{
    uint64_t perfCounterAddr = m_RegBase + READ_CNTR(client_no, bank);
    return READ_UINT32(perfCounterAddr);
}
