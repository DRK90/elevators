#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

// Constants
const int NUM_ELEVATORS = 4;
const int NUM_FLOORS = 100;
const int MAX_CAPACITY = 8;
const int INITIAL_SPEED = 10;
const int IMPROVED_SPEED = 5;

class Passenger
{
public:
    int startTime;
    int startFloor;
    int endFloor;
    int waitTime;
    int travelTime;

    Passenger(int _startTime, int _startFloor, int _endFloor)
        : startTime(_startTime), startFloor(_startFloor), endFloor(_endFloor), waitTime(0), travelTime(0) {}
};

class Floor
{
public:
    std::queue<Passenger *> waitingPassengers;

    void addPassenger(Passenger *passenger)
    {
        waitingPassengers.push(passenger);
    }
};

class Elevator {
public:
    enum State { STOPPED, STOPPING, MOVING_UP, MOVING_DOWN };

    int currentFloor;
    State state;
    int stoppingTime;
    int floorTravelTime;
    std::vector<Passenger*> passengers;

    Elevator(int _floorTravelTime)
        : currentFloor(0), state(STOPPED), stoppingTime(0), floorTravelTime(_floorTravelTime) {}

    void update(std::vector<Floor>& floors, int currentTime) {
        switch (state) {
            case STOPPED:
                if (!floors[currentFloor].waitingPassengers.empty()) {
                    addPassengersFromFloor(floors[currentFloor]);
                }
                updateState(floors, currentTime);
                break;

            case STOPPING:
                stoppingTime--;
                if (stoppingTime == 0) {
                    state = STOPPED;
                }
                break;

            case MOVING_UP:
                currentFloor++;
                updateState(floors, currentTime);
                break;

            case MOVING_DOWN:
                currentFloor--;
                updateState(floors, currentTime);
                break;
        }
    }

    void addPassengersFromFloor(Floor& floor) {
        while (!floor.waitingPassengers.empty() && passengers.size() < MAX_CAPACITY) {
            passengers.push_back(floor.waitingPassengers.front());
            floor.waitingPassengers.pop();
        }
    }

    void updateState(std::vector<Floor>& floors, int currentTime) {
        if (state == STOPPED) {
            bool passengersToDischarge = std::any_of(passengers.begin(), passengers.end(), [this](Passenger* p) {
                return p->endFloor == currentFloor;
            });
            if (passengersToDischarge) {
                passengers.erase(std::remove_if(passengers.begin(), passengers.end(),
                                                [this](Passenger* p) {
                    if (p->endFloor == currentFloor) {
                        p->travelTime += p->waitTime;
                        return true;
                    }
                    return false;
                }),
                                 passengers.end());
                state = STOPPING;
                stoppingTime = 2;
                return;
            }

            for (Passenger* p : passengers) {
                p->waitTime++;
            }

            if (currentFloor < NUM_FLOORS - 1) {
                state = MOVING_UP;
            } else {
                state = MOVING_DOWN;
            }
        } else if (state == MOVING_UP || state == MOVING_DOWN) {
            if (currentFloor > 0 && currentFloor < NUM_FLOORS - 1) {
                if (!floors[currentFloor].waitingPassengers.empty() ||
                    std::any_of(passengers.begin(), passengers.end(), [this](Passenger* p) {
                        return p->endFloor == currentFloor;
                    })) {
                    state = STOPPING;
                    stoppingTime = 2;
                }
            } else {
                state = (state == MOVING_UP) ? MOVING_DOWN : MOVING_UP;
            }
        }
    }
};

    std::vector<Passenger> readPassengersFromFile(const std::string &filename)
    {
        std::vector<Passenger> passengers;
        std::ifstream inputFile(filename);
        std::string line;
        while (std::getline(inputFile, line))
        {
            std::vector<std::string> tokens;
            boost::algorithm::split(tokens, line, boost::is_any_of(","));

            int startTime = std::stoi(tokens[0]);
            int startFloor = std::stoi(tokens[1]);
            int endFloor = std::stoi(tokens[2]);

            passengers.emplace_back(startTime, startFloor, endFloor);
        }

        return passengers;
    }

    void runSimulation(std::vector<Passenger> &passengers, int floorTravelTime)
    {
        std::vector<Elevator> elevators(NUM_ELEVATORS, Elevator(floorTravelTime));
        std::vector<Floor> floors(NUM_FLOORS);
        for (Passenger &p : passengers)
        {
            floors[p.startFloor].addPassenger(&p);
        }

        int currentTime = 0;
        while (true)
        {
            bool allPassengersArrived = std::all_of(passengers.begin(), passengers.end(),
                                                    [](const Passenger &p)
                                                    { return p.travelTime > 0; });
            if (allPassengersArrived)
            {
                break;
            }

            for (Elevator &elevator : elevators)
            {
                elevator.update(floors, currentTime);
            }

            currentTime++;
        }
    }

