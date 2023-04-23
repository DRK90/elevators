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

class Passenger {
public:
    enum Status { WAITING, PICKED_UP };

    int startTime;
    int startFloor;
    int endFloor;
    int waitTime;
    int travelTime;
    Status status;

    Passenger(int _startTime, int _startFloor, int _endFloor)
        : startTime(_startTime), startFloor(_startFloor), endFloor(_endFloor), waitTime(0), travelTime(0), status(WAITING) {}
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
        Passenger* p = floor.waitingPassengers.front();
        floor.waitingPassengers.pop();
        passengers.push_back(p);
        p->status = Passenger::Status::PICKED_UP;
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
            std::cout<<startTime<<" "<<startFloor<<" "<<endFloor<<"\n";
        }

        return passengers;
    }

bool hasElevatorMoving(const std::vector<Elevator>& elevators) {
    for (const Elevator& elevator : elevators) {
        if (elevator.state == Elevator::State::MOVING_UP || elevator.state == Elevator::State::MOVING_DOWN) {
            return true;
        }
    }
    return false;
}

void runSimulation(std::vector<Passenger>& passengers, const std::vector<Elevator>& initialElevators, int floorTravelTime) {
    std::vector<Floor> floors(NUM_FLOORS);
    for (Passenger& passenger : passengers) {
        floors[passenger.startFloor].waitingPassengers.push(&passenger);
    }

    std::vector<Elevator> elevators = initialElevators;
    for (Elevator& elevator : elevators) {
        elevator.floorTravelTime = floorTravelTime;
    }

    int currentTime = 0;

    while (!passengers.empty() || hasElevatorMoving(elevators)) {
        for (Elevator& elevator : elevators) {
            elevator.update(floors, currentTime);
        }

        for (Passenger& passenger : passengers) {
            if (passenger.status == Passenger::Status::WAITING) {
                passenger.waitTime++;
            }
        }

        currentTime++;

        // Remove passengers who have reached their destination
        passengers.erase(std::remove_if(passengers.begin(), passengers.end(), [](const Passenger& p) {
            return p.status == Passenger::Status::PICKED_UP && p.waitTime + p.travelTime <= currentTime;
        }), passengers.end());
    }

    double averageWaitTime = 0;
    double averageTravelTime = 0;

    for (const Passenger& passenger : passengers) {
        averageWaitTime += passenger.waitTime;
        averageTravelTime += passenger.travelTime;
    }

    averageWaitTime /= passengers.size();
    averageTravelTime /= passengers.size();

    std::cout << "Floor Travel Time: " << floorTravelTime << " seconds" << std::endl;
    std::cout << "Average Wait Time: " << averageWaitTime << " seconds" << std::endl;
    std::cout << "Average Travel Time: " << averageTravelTime << " seconds" << std::endl;
}


