#include "bru_manager.h"


/*
    commands
*/


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
        ");
    }





    int opt;
    BRUManager* bru = new BRUManager(4, 4, 4, true);

    uint32_t client = 0;
    uint16_t domain = 0;
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
    while ((opt = getopt(argc, argv, "e:q:p:x:c:d:m:r:")) != -1) {
        switch(opt)
        {
            case 'e':
            {
                bool en = std::string(optarg) == "true";
                bru->EnableGlobal(en);
                break;
            }
            case 'q':
            {
                bool en = std::string(optarg) == "true";
                bru->EnablePerformanceCounters(en);
                break;
            }
            case 'p':
            {
                uint32_t period = atol(optarg);
                bru->SetRegulationPeriod(period);
                break;
            }
            case 'x':
            {
                uint32_t period = atol(optarg);
                bru->SetPerformanceCountersPeriod(period);
                break;
            }
            case 'c':
            {
                client = atol(optarg);
                break;
            }
            case 'd':
            {
                domain = atol(optarg);
                bru->SetClientDomainID(client, domain);
                break;
            }
            case 'm':
            {
                uint32_t max_reads = atol(optarg);
                bru->SetDomainMaxReads(domain, max_reads);
                break;
            }
            case 'r':
            {
                bool en = std::string(optarg) == "true";
                bru->EnableClientRegulation(client, en);
                break;
            }
        }
    }

}