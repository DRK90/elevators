#include "classes.hpp"

int main() {
std::vector<Passenger> passengers = readPassengersFromFile("test.csv");
std::vector<Passenger> initialPassengers = passengers;
runSimulation(initialPassengers, INITIAL_SPEED);
double initialAverageWait = 0.0;
double initialAverageTravel = 0.0;

for (const Passenger& p : initialPassengers) {
    std::cout<< p.waitTime << " \n";
    std::cout<< p.travelTime << " \n";
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
improvedAverageTravel /= improvedPassengers.size();

double increaseAverageWait = improvedAverageWait / initialAverageWait;
double increaseAverageTravel = improvedAverageTravel / initialAverageTravel;

std::cout << "Init ave Wait: " << initialAverageWait << "\n" << "init ave Travel: " << initialAverageTravel << "\n";
std::cout << "improved ave wait: " << improvedAverageWait << "\n" << "improved ave Travel: " << improvedAverageTravel << "\n";
std::cout << "increaseAverageWait: " << increaseAverageWait << "\n" << "increaseAverageTravel: " << increaseAverageTravel << "\n";
}
