#include <bits/stdc++.h>
#include "parser.h"

#define all(v) v.begin(), v.end()

using namespace std;

/** Global Constants **/
const string TRACE = "trace";
const string SHOW_STATS = "stats";
const string ALGORITHMS[5] = {"", "FCFS", "RR-", "SRT", "AGING"};

bool compareByServiceTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<2>(a) < get<2>(b));
}
bool compareByArrivalTime(const tuple<string, int, int> &a, const tuple<string, int, int> &b)
{
    return (get<1>(a) < get<1>(b));
}
bool compareByResponseRatioDescending(tuple<string, double, int> a, tuple<string, double, int> b)
{
    return get<1>(a) > get<1>(b);
}
bool compareByPriority(const tuple<int, int, int> &a, const tuple<int, int, int> &b)
{
    if (get<0>(a) == get<0>(b))
        return get<2>(a) > get<2>(b);
    return get<0>(a) > get<0>(b);
}

void resetTimeline()
{
    for (int i = 0; i < last_instant; i++)
        for (int j = 0; j < process_count; j++)
            timeline[i][j] = ' ';
}

string getProcessName(tuple<string, int, int> &a)
{
    return get<0>(a);
}

int getArrivalTime(tuple<string, int, int> &a)
{
    return get<1>(a);
}

int getServiceTime(tuple<string, int, int> &a)
{
    return get<2>(a);
}

int getPriorityLevel(tuple<string, int, int> &a)
{
    return get<2>(a);
}

double calculate_response_ratio(int wait_time, int service_time)
{
    return (wait_time + service_time) * 1.0 / service_time;
}

void fillInWaitTime()
{
    for (int i = 0; i < process_count; i++)
    {
        int arrivalTime = getArrivalTime(processes[i]);
        for (int k = arrivalTime; k < finishTime[i]; k++)
        {
            if (timeline[k][i] != '*')
                timeline[k][i] = '.';
        }
    }
}

void firstComeFirstServe()
{
    int currentTime = getArrivalTime(processes[0]);
    for (int i = 0; i < process_count; i++)
    {
        int processIndex = i;
        int arrivalTime = getArrivalTime(processes[i]);
        int serviceTime = getServiceTime(processes[i]);

        finishTime[processIndex] = (currentTime + serviceTime);
        turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
        normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);

        for (int j = currentTime; j < finishTime[processIndex]; j++)
            timeline[j][processIndex] = '*';
        for (int j = arrivalTime; j < currentTime; j++)
            timeline[j][processIndex] = '.';
        currentTime += serviceTime;
    }
}

void roundRobin(int originalQuantum)
{
    queue<pair<int, int>> q;
    int j = 0;
    if (getArrivalTime(processes[j]) == 0)
    {
        q.push(make_pair(j, getServiceTime(processes[j])));
        j++;
    }
    int currentQuantum = originalQuantum;
    for (int currentTime = 0; currentTime < last_instant; currentTime++)
    {
        if (!q.empty())
        {
            int processIndex = q.front().first;
            q.front().second = q.front().second - 1;
            int remainingServiceTime = q.front().second;
            int arrivalTime = getArrivalTime(processes[processIndex]);
            int serviceTime = getServiceTime(processes[processIndex]);
            currentQuantum--;
            timeline[currentTime][processIndex] = '*';
            while (j < process_count && getArrivalTime(processes[j]) == currentTime + 1)
            {
                q.push(make_pair(j, getServiceTime(processes[j])));
                j++;
            }

            if (currentQuantum == 0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = currentTime + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                currentQuantum = originalQuantum;
                q.pop();
            }
            else if (currentQuantum == 0 && remainingServiceTime != 0)
            {
                q.pop();
                q.push(make_pair(processIndex, remainingServiceTime));
                currentQuantum = originalQuantum;
            }
            else if (currentQuantum != 0 && remainingServiceTime == 0)
            {
                finishTime[processIndex] = currentTime + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
                q.pop();
                currentQuantum = originalQuantum;
            }
        }
        while (j < process_count && getArrivalTime(processes[j]) == currentTime + 1)
        {
            q.push(make_pair(j, getServiceTime(processes[j])));
            j++;
        }
    }
    fillInWaitTime();
}

void shortestRemainingTime()
{
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    int j = 0;
    for (int i = 0; i < last_instant; i++)
    {
        while (j < process_count && getArrivalTime(processes[j]) == i)
        {
            pq.push(make_pair(getServiceTime(processes[j]), j));
            j++;
        }
        if (!pq.empty())
        {
            int processIndex = pq.top().second;
            int remainingTime = pq.top().first;
            pq.pop();
            int serviceTime = getServiceTime(processes[processIndex]);
            int arrivalTime = getArrivalTime(processes[processIndex]);
            timeline[i][processIndex] = '*';

            if (remainingTime == 1) // process finished
            {
                finishTime[processIndex] = i + 1;
                turnAroundTime[processIndex] = (finishTime[processIndex] - arrivalTime);
                normTurn[processIndex] = (turnAroundTime[processIndex] * 1.0 / serviceTime);
            }
            else
            {
                pq.push(make_pair(remainingTime - 1, processIndex));
            }
        }
    }
    fillInWaitTime();
}

void aging(int originalQuantum)
{
    vector<tuple<int, int, int>> v; // tuple of priority level, process index and total waiting time
    int j = 0, currentProcess = -1;
    for (int currentTime = 0; currentTime < last_instant; currentTime++)
    {
        while (j < process_count && getArrivalTime(processes[j]) <= currentTime)
        {
            v.push_back(make_tuple(getPriorityLevel(processes[j]), j, 0));
            j++;
        }

        for (int i = 0; i < v.size(); i++)
        {
            if (get<1>(v[i]) == currentProcess)
            {
                get<2>(v[i]) = 0;
                get<0>(v[i]) = getPriorityLevel(processes[currentProcess]);
            }
            else
            {
                get<0>(v[i])++;
                get<2>(v[i])++;
            }
        }
        sort(v.begin(), v.end(), compareByPriority);
        currentProcess = get<1>(v[0]);
        int currentQuantum = originalQuantum;
        while (currentQuantum-- && currentTime < last_instant)
        {
            timeline[currentTime][currentProcess] = '*';
            currentTime++;
        }
        currentTime--;
    }
    fillInWaitTime();
}

void printAlgorithm(int algorithm_index)
{
    int algorithm_id = algorithms[algorithm_index].first - '0';
    if (algorithm_id == 2)
        cout << ALGORITHMS[algorithm_id] << algorithms[algorithm_index].second << endl;
    else
        cout << ALGORITHMS[algorithm_id] << endl;
}

void printProcesses()
{
    cout << "Process    ";
    for (int i = 0; i < process_count; i++)
        cout << "|  " << getProcessName(processes[i]) << "  ";
    cout << "|\n";
}
void printArrivalTime()
{
    cout << "Arrival    ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ", getArrivalTime(processes[i]));
    cout << "|\n";
}
void printServiceTime()
{
    cout << "Service    |";
    for (int i = 0; i < process_count; i++)
        printf("%3d  |", getServiceTime(processes[i]));
    cout << " Mean|\n";
}
void printFinishTime()
{
    cout << "Finish     ";
    for (int i = 0; i < process_count; i++)
        printf("|%3d  ", finishTime[i]);
    cout << "|-----|\n";
}
void printTurnAroundTime()
{
    cout << "Turnaround |";
    int sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        printf("%3d  |", turnAroundTime[i]);
        sum += turnAroundTime[i];
    }
    if ((1.0 * sum / turnAroundTime.size()) >= 10)
        printf("%2.2f|\n", (1.0 * sum / turnAroundTime.size()));
    else
        printf(" %2.2f|\n", (1.0 * sum / turnAroundTime.size()));
}

void printNormTurn()
{
    cout << "NormTurn   |";
    float sum = 0;
    for (int i = 0; i < process_count; i++)
    {
        if (normTurn[i] >= 10)
            printf("%2.2f|", normTurn[i]);
        else
            printf(" %2.2f|", normTurn[i]);
        sum += normTurn[i];
    }

    if ((1.0 * sum / normTurn.size()) >= 10)
        printf("%2.2f|\n", (1.0 * sum / normTurn.size()));
    else
        printf(" %2.2f|\n", (1.0 * sum / normTurn.size()));
}
void printStats(int algorithm_index)
{
    printAlgorithm(algorithm_index);
    printProcesses();
    printArrivalTime();
    printServiceTime();
    printFinishTime();
    printTurnAroundTime();
    printNormTurn();
}

void printTimeline(int algorithm_index)
{
    for (int i = 0; i <= last_instant; i++)
        cout << i % 10 << " ";
    cout << "\n";
    cout << "------------------------------------------------\n";
    for (int i = 0; i < process_count; i++)
    {
        cout << getProcessName(processes[i]) << "     |";
        for (int j = 0; j < last_instant; j++)
        {
            cout << timeline[j][i] << "|";
        }
        cout << " \n";
    }
    cout << "------------------------------------------------\n";
}

void execute_algorithm(char algorithm_id, int quantum, string operation)
{
    switch (algorithm_id)
    {
    case '1':
        if (operation == TRACE)
            cout << "FCFS  ";
        firstComeFirstServe();
        break;
    case '2':
        if (operation == TRACE)
            cout << "RR-" << quantum << "  ";
        roundRobin(quantum);
        break;
    case '3':
        if (operation == TRACE)
            cout << "SRT   ";
        shortestRemainingTime();
        break;
    case '4':
        if (operation == TRACE)
            cout << "Aging ";
        aging(quantum);
        break;
    default:
        break;
    }
}

int main()
{
    parse();
    for (int idx = 0; idx < (int)algorithms.size(); idx++)
    {
        resetTimeline();
        execute_algorithm(algorithms[idx].first, algorithms[idx].second, operation);
        if (operation == TRACE)
            printTimeline(idx);
        else if (operation == SHOW_STATS)
            printStats(idx);
        cout << "\n";
    }
    return 0;
}
