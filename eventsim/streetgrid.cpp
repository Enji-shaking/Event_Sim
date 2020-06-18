#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include "streetgrid.h"
#include "derived_events.h"

using namespace std;
StreetGrid::StreetGrid(
    const std::vector<unsigned int> rowCapacities,
    const std::vector<unsigned int> colCapacities)
    : rowCapacities_(rowCapacities), colCapacities_(colCapacities)
{
    n = rowCapacities.size();
    m = colCapacities.size();
    expectedVehicles_ = 0;

    // TO BE COMPLETED
    street_ = vector<vector<vector<int> > >(n, vector<vector<int>> (m, vector<int>(5, 0) ) );
    // rowOccupancy, colOccupancy, rowBlock (0 no, 1 yes), colBlock, choseRow(0 no, 1 yes)





    // Call monitor state to output starting state
    monitorState();
}

size_t StreetGrid::numRows() const
{
    return n;
}
size_t StreetGrid::numCols() const
{
    return m;
}
void StreetGrid::setExpected(size_t expectedVehicles)
{
    expectedVehicles_ = expectedVehicles;
}

void StreetGrid::outputCompletedVehicles(std::ostream& ostr) const
{
    for(auto v : completed_) {
        ostr << v.end << " " << v.id << std::endl;
    }
}

bool StreetGrid::allVehiclesArrived() const
{
    return completed_.size() == expectedVehicles_;
}

void StreetGrid::monitorState()
{
    std::cout << "State: " << std::endl;
    std::cout << std::setw(4) << " ";

    // Print header row with column capacities
    for(size_t cidx = 0; cidx < m; ++cidx) {
        std::cout <<  std::setw(4) << colCapacities_[cidx] << " " <<  std::setw(4) << " ";
    }
    std::cout << std::endl;
    std::cout << "    ";
    for(size_t cidx = 0; cidx < m; ++cidx) {
        std::cout <<  std::setw(4) << "====="  << "=====";
    }
    std::cout << std::endl;

    // Start printing grid data
    for(size_t ridx = 0; ridx < n; ++ridx) {
        // Print row capacity
        std::cout << std::setw(2) << rowCapacities_[ridx] << " |";
        // Print row occupancies
        for(size_t cidx = 0; cidx < m; ++cidx) {
            std::cout << std::setw(4) << "+" << " ";
            if(cidx < m-1) {
                if(isBlocked(ridx,cidx,true)) {
                    std::cout << std::setw(3) << getRowOccupancy(ridx,cidx);
                    std::cout << "B";
                }
                else {
                    std::cout << std::setw(4) << getRowOccupancy(ridx,cidx);
                }
            }
        }
        std::cout << std::endl;
        std::cout <<  "   |";
        // Print column occupancies
        if(ridx < n-1) {
            for(size_t cidx = 0; cidx < m; ++cidx) {
                if(isBlocked(ridx,cidx,false)) {
                    std::cout << std::setw(3) << getColOccupancy(ridx,cidx);
                    std::cout << "B";
                }
                else {
                    std::cout << std::setw(4) << getColOccupancy(ridx,cidx);
                }
                std::cout << std::setw(4) << " " << " " ;
            }
        }
        std::cout << std::endl;

    }
}

size_t StreetGrid::getRowOccupancy(size_t row, size_t col) const
{
    // TO BE COMPLETED
    return street_[row][col][0];

}

size_t StreetGrid::getColOccupancy(size_t row, size_t col) const
{
    // TO BE COMPLETED
    return street_[row][col][1];

}

bool StreetGrid::isBlocked(size_t row, size_t col, bool rowDir) const
{
    // TO BE COMPLETED
    if(rowDir) return street_[row][col][2];
    else return street_[row][col][3];

}

bool StreetGrid::shouldChooseRow(size_t row, size_t col)
{
    // TO BE COMPLETED
    if(isBlocked(row, col, true)||col==m-1){
        return false;
    }
    if(isBlocked(row, col, false)||row==n-1){
        return true;
    }
    int timeRow = timeToTravel(row, col, true);
    int timeCol = timeToTravel(row, col, false);
    // update the record of choice made before
    if(timeRow == timeCol){
        street_[row][col][4] = !street_[row][col][4];
        return street_[row][col][4];
    }
    return timeRow < timeCol;
}

int StreetGrid::timeToTravel(size_t row, size_t col, bool rowDir) const
{

    int segCapa, segOcup;
    if(rowDir){
        segCapa = rowCapacities_[row];
        segOcup = getRowOccupancy(row, col);
    }else{
        segCapa = colCapacities_[col];
        segOcup = getColOccupancy(row, col);
    }
    return SCALE_FACTOR*max(1.0, ((1+segOcup)/(double)segCapa ) );
}


void StreetGrid::setBlockage(size_t row, size_t col, bool rowDir, bool val)
{
    // TO BE COMPLETED
    if(rowDir) street_[row][col][2] = val;
    else street_[row][col][3] = val;
    cout << "BlockageEvent::process()"<<endl;
    cout << (val?"Blocking ":"Unblocking ")<< (val?"row":"col") << " at "<<row<<","<<col<<endl;
}

EventList StreetGrid::processArrival(const std::string& vehicleID, const TimeStamp& ts)
{
    // TO BE COMPLETED
    EventList eventGenerated;
    if(vehicles_.find(vehicleID)==vehicles_.end()) throw logic_error("No vehicle with that ID exists");
    Vehicle* temp = &vehicles_[vehicleID];
    street_[temp->rowIndex][temp->colIndex][(temp->rowDir?0:1)]--;
    temp->rowDir? temp->colIndex++ : temp->rowIndex++;
    cout<<"StreetGrid::processArrival - vehicle "<<temp->id<<" now at " <<temp->rowIndex<<","<<temp->colIndex<<endl;
    if(reachEnd(temp)){
        vehicles_[vehicleID].end = ts;
        completed_.push_back(vehicles_[vehicleID]);
        vehicles_.erase(vehicleID);
        cout << "StreetGrid::processArrival - At terminal location" <<endl;
    }
    else{
        temp->rowDir = shouldChooseRow(temp->rowIndex, temp->colIndex);
        int time = timeToTravel(temp->rowIndex, temp->colIndex, temp->rowDir);
        auto event = new ArrivalEvent(ts+time, *this, vehicleID);
        eventGenerated.push_back(event);

        street_[temp->rowIndex][temp->colIndex][(temp->rowDir?0:1)] ++;
        cout<<"StreetGrid::processArrival - vehicle "<<temp->id<<" will go "<<(temp->rowDir?"row":"col")
            <<" = "<<time<<endl;
    }

    return eventGenerated;

}

EventList StreetGrid::addVehicle(const Vehicle& v)
{
    // TO BE COMPLETED
    if(vehicles_.find(v.id)!=vehicles_.end()) throw logic_error("Vehicle ID Exists");
//    v.rowDir = shouldChooseRow(v.rowIndex, v.colIndex);
    auto vehicle = v;
    cout<<"StreetGrid::AddVehicle - vehicle "<<vehicle.id<<" now at " <<vehicle.rowIndex<<","<<vehicle.colIndex<<endl;
    vehicle.rowDir = shouldChooseRow(vehicle.rowIndex, vehicle.colIndex);
    vehicles_.insert(make_pair(vehicle.id,vehicle));
    EventList eventGenerated;
    int time = timeToTravel(vehicle.rowIndex, vehicle.colIndex, vehicle.rowDir);
    // time needs to be updated
    auto event = new ArrivalEvent(v.start+time, *this, v.id);
    eventGenerated.push_back(event);

    street_[vehicle.rowIndex][vehicle.colIndex][(vehicle.rowDir?0:1)] ++;
    cout<<"StreetGrid::AddVehicle - vehicle "<<vehicle.id<<" will go "<<(vehicle.rowDir?"row":"col")
        <<" = "<<time<<endl;
    return eventGenerated;


}

bool StreetGrid::reachEnd(Vehicle* car) const {
    return (car->rowIndex == street_.size() - 1 && car->colIndex == street_[0].size() - 1);
}


