#include "bru_manager.h"


/*
    commands
*/
enum BRU_OP {
    GLOBAL_EN,
    GLOBAL_PERF_EN,
    SET_PERIOD,
    SET_PERIOD_PERF,
    SELECT_CLIENT,
    CLIENT_ENABLE_REGULATION,
    SELECT_CLIENT_DOMAIN,
    SET_MAX_READS_DOMAIN
};


template<typename T>
struct BRUTask
{
    BRUTask(BRU_OP op, T data) : op(op), data(data) {}
    BRU_OP op;
    T data;
};



int main(int argc, char** argv)
{
    if (argc < 2){
        printf("\
        -e --> enable global                    {true|false}\n\
        -q --> enable performance counters      {true|false}\n\
        -p --> set regulation period            {unsigned long period}\n\
        -x --> set performance counters period  {unsigned long period}\n\
        -r --> enable client regulation         {true|false}\n\
        -c --> select client                    {client_no}\n\
        -d --> select client domain             {domain_id}\n\
        -m --> set max reads domain             {unsigned long max_reads}\n\
        -g --> enable debug\
        ");
    }





    int opt;
    
    std::vector<BRU_OP*> operations;
    uint32_t client = 0;
    uint16_t domain = 0;
    bool debug = false;
    /*
        -e --> enable global                    {true|false}
        -q --> enable performance counters      {true|false}
        -p --> set regulation period            {unsigned long period}
        -x --> set performance counters period  {unsigned long period}
        -r --> enable client regulation         {true|false}
        -c --> select client                    {client_no}
        -d --> select client domain             {domain_id}
        -m --> set max reads domain             {unsigned long max_reads}
    */
    while ((opt = getopt(argc, argv, "e:q:p:x:c:d:m:r:g:")) != -1) {
        switch(opt)
        {
            case 'e':
            {
                bool en = std::string(optarg) == "true";
                auto task = new BRUTask<bool>(BRU_OP::GLOBAL_EN, en);
                operations.push_back(&task->op);
                break;
            }
            case 'q':
            {
                bool en = std::string(optarg) == "true";
                auto task = new BRUTask<bool>(BRU_OP::GLOBAL_PERF_EN, en);
                operations.push_back(&task->op);
                break;
            }
            case 'p':
            {
                uint32_t period = atol(optarg);
                auto task = new BRUTask<uint32_t>(BRU_OP::SET_PERIOD, period);
                operations.push_back(&task->op);
                break;
            }
            case 'x':
            {
                uint32_t period = atol(optarg);
                auto task = new BRUTask<uint32_t>(BRU_OP::SET_PERIOD_PERF, period);;
                operations.push_back(&task->op);
                break;
            }
            case 'c': // We want client and domain to be selected before we perform the other operations
            {
                client = atol(optarg);
                break;
            }
            case 'd': // We want client and domain to be selected before we perform the other operations
            {
                domain = atol(optarg);
                auto task = new BRUTask<uint16_t>(BRU_OP::SELECT_CLIENT_DOMAIN, domain);
                operations.push_back(&task->op);
                break;
            }
            case 'm':
            {
                uint32_t max_reads = atol(optarg);
                auto task = new BRUTask<uint32_t>(BRU_OP::SET_MAX_READS_DOMAIN, max_reads);
                operations.push_back(&task->op);
                break;
            }
            case 'r':
            {
                bool en = std::string(optarg) == "true";
                auto task = new BRUTask<bool>(BRU_OP::CLIENT_ENABLE_REGULATION, en);
                operations.push_back(&task->op);
                break;
            }
            case 'g':
            {
                debug = true;
                break;
            }
        }
    }


    BRUManager* bru = new BRUManager(debug=debug);

    /*
        Now that client and domain are set, we can properly perform the collected operations
    */
    for (auto& p_op : operations)
    {
        auto op = *p_op;
        switch (op)
        {
            case BRU_OP::GLOBAL_EN:
            {
                auto task = (BRUTask<bool>*)p_op;
                bru->EnableGlobal(task->data);
                break;
            }
            case BRU_OP::GLOBAL_PERF_EN:
            {
                auto task = (BRUTask<bool>*)p_op;
                bru->EnablePerformanceCounters(task->data);
                break;
            }
            case BRU_OP::SET_PERIOD:
            {
                auto task = (BRUTask<uint32_t>*)p_op;
                bru->SetRegulationPeriod(task->data);
                break;
            }
            case BRU_OP::SET_PERIOD_PERF:
            {
                auto task = (BRUTask<uint32_t>*)p_op;
                bru->SetPerformanceCountersPeriod(task->data);
                break;
            }
            case BRU_OP::SELECT_CLIENT_DOMAIN:
            {
                auto task = (BRUTask<uint16_t>*)p_op;
                bru->SetClientDomainID(client, task->data);
                break;
            }
            case BRU_OP::CLIENT_ENABLE_REGULATION:
            {
                auto task = (BRUTask<bool>*)p_op;
                bru->EnableClientRegulation(client, task->data);
                break;
            }
            case BRU_OP::SET_MAX_READS_DOMAIN:
            {
                auto task = (BRUTask<uint32_t>*)p_op;
                bru->SetDomainMaxReads(domain, task->data);
                break;
            }
            default:
                break;
        }
        delete p_op;
    }



}