#include "classes.hpp"

int main() {
std::vector<Passenger> passengers = readPassengersFromFile("Elevators.csv");
std::vector<Passenger> initialPassengers = passengers;
runSimulation(initialPassengers, INITIAL_SPEED);
double initialAverageWait = 0.0;
double initialAverageTravel = 0.0;

for (const Passenger& p : initialPassengers) {
    initialAverageWait += p.waitTime;
    initialAverageTravel += p.travelTime;
}

initialAverageWait /= initialPassengers.size();
initialAverageTravel /= initialPassengers.size();

std::vector<Passenger> improvedPassengers = passengers;
runSimulation(improvedPassengers, IMPROVED_SPEED);
double improvedAverageWait = 0.0;
double improvedAverageTravel = 0.0;

for (const Passenger& p : improvedPassengers) {
    improvedAverageWait += p.waitTime;
    improvedAverageTravel += p.travelTime;
}

improvedAverageWait /= improvedPassengers.size();

std::cout << "Init ave Wait: " << initialAverageWait << "\n" << "init ave Travel: " << initialAverageTravel << "\n";
}
