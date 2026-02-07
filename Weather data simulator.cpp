

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <random>
#include <algorithm>

struct WeatherData {
    time_t timestamp;
    double temperature;    // Celsius
    double humidity;       // Percentage
    double pressure;       // hPa
    double windSpeed;      // km/h
    std::string condition; // "Sunny", "Rainy", etc.
    
    WeatherData(time_t ts, double temp, double hum, double pres, 
                double wind, const std::string& cond)
        : timestamp(ts), temperature(temp), humidity(hum), 
          pressure(pres), windSpeed(wind), condition(cond) {}
    
    void display() const {
        struct tm* timeinfo = localtime(&timestamp);
        std::cout << std::put_time(timeinfo, "%Y-%m-%d %H:%M") << " | "
                  << std::setw(6) << std::fixed << std::setprecision(1) << temperature << "°C | "
                  << std::setw(5) << std::setprecision(0) << humidity << "% | "
                  << std::setw(7) << std::setprecision(0) << pressure << " hPa | "
                  << std::setw(5) << std::setprecision(1) << windSpeed << " km/h | "
                  << std::setw(10) << condition << " |";
        
        // Add emoji based on condition
        if (condition == "Sunny") std::cout << " ☀️";
        else if (condition == "Cloudy") std::cout << " ☁️";
        else if (condition == "Rainy") std::cout << " 🌧️";
        else if (condition == "Stormy") std::cout << " ⛈️";
        else if (condition == "Snowy") std::cout << " ❄️";
        else if (condition == "Foggy") std::cout << " 🌫️";
        
        std::cout << "\n";
    }
    
    std::string getTempCategory() const {
        if (temperature < 0) return "Freezing";
        else if (temperature < 10) return "Cold";
        else if (temperature < 20) return "Cool";
        else if (temperature < 30) return "Warm";
        else return "Hot";
    }
    
    double calculateHeatIndex() const {
        // Simplified heat index calculation
        if (temperature < 27) return temperature;
        
        double c1 = -8.78469475556;
        double c2 = 1.61139411;
        double c3 = 2.33854883889;
        double c4 = -0.14611605;
        double c5 = -0.012308094;
        double c6 = -0.0164248277778;
        double c7 = 0.002211732;
        double c8 = 0.00072546;
        double c9 = -0.000003582;
        
        double T = temperature;
        double R = humidity;
        
        return c1 + c2*T + c3*R + c4*T*R + c5*T*T + 
               c6*R*R + c7*T*T*R + c8*T*R*R + c9*T*T*R*R;
    }
};

class WeatherStation {
private:
    std::string location;
    std::vector<WeatherData> readings;
    std::map<std::string, int> conditionCount;
    
public:
    WeatherStation(const std::string& loc) : location(loc) {}
    
    void addReading(const WeatherData& data) {
        readings.push_back(data);
        conditionCount[data.condition]++;
    }
    
    void generateRandomReadings(int days) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> tempDist(-10, 40);
        std::uniform_real_distribution<> humDist(30, 100);
        std::uniform_real_distribution<> pressDist(980, 1050);
        std::uniform_real_distribution<> windDist(0, 50);
        
        std::vector<std::string> conditions = {
            "Sunny", "Cloudy", "Rainy", "Stormy", "Snowy", "Foggy"
        };
        std::discrete_distribution<> condDist({40, 30, 15, 5, 5, 5}); // Probabilities
        
        time_t now = time(0);
        
        for (int i = 0; i < days * 24; i++) { // One reading per hour
            time_t timestamp = now - (days * 24 - i) * 3600;
            
            double temp = tempDist(gen);
            double humidity = humDist(gen);
            double pressure = pressDist(gen);
            double wind = windDist(gen);
            
            // Adjust based on condition
            std::string condition = conditions[condDist(gen)];
            if (condition == "Rainy" || condition == "Stormy") {
                humidity = std::min(100.0, humidity + 20);
            }
            if (condition == "Snowy") {
                temp = std::min(0.0, temp - 5);
            }
            
            addReading(WeatherData(timestamp, temp, humidity, pressure, wind, condition));
        }
    }
    
    void displayRecentReadings(int count = 10) {
        std::cout << "\n=== RECENT WEATHER READINGS (" << location << ") ===\n";
        std::cout << "Timestamp           | Temp  | Hum  | Pressure | Wind  | Condition  |\n";
        std::cout << std::string(70, '-') << "\n";
        
        int start = std::max(0, (int)readings.size() - count);
        for (int i = start; i < readings.size(); i++) {
            readings[i].display();
        }
    }
    
    void displayStatistics() {
        if (readings.empty()) {
            std::cout << "No weather data available.\n";
            return;
        }
        
        std::cout << "\n=== WEATHER STATISTICS (" << location << ") ===\n";
        std::cout << "Total Readings: " << readings.size() << "\n";
        std::cout << "Data Period: " << readings.size() / 24 << " days\n\n";
        
        // Calculate averages and extremes
        double sumTemp = 0, sumHum = 0, sumPress = 0, sumWind = 0;
        double maxTemp = readings[0].temperature;
        double minTemp = readings[0].temperature;
        time_t hottestTime = readings[0].timestamp;
        time_t coldestTime = readings[0].timestamp;
        
        std::map<std::string, double> tempByCategory;
        std::map<std::string, int> countByCategory;
        
        for (const auto& reading : readings) {
            sumTemp += reading.temperature;
            sumHum += reading.humidity;
            sumPress += reading.pressure;
            sumWind += reading.windSpeed;
            
            if (reading.temperature > maxTemp) {
                maxTemp = reading.temperature;
                hottestTime = reading.timestamp;
            }
            if (reading.temperature < minTemp) {
                minTemp = reading.temperature;
                coldestTime = reading.timestamp;
            }
            
            std::string category = reading.getTempCategory();
            tempByCategory[category] += reading.temperature;
            countByCategory[category]++;
        }
        
        std::cout << "Temperature:\n";
        std::cout << "  Average: " << std::fixed << std::setprecision(1) 
                 << sumTemp / readings.size() << "°C\n";
        std::cout << "  Maximum: " << maxTemp << "°C on "
                 << std::put_time(localtime(&hottestTime), "%Y-%m-%d %H:%M") << "\n";
        std::cout << "  Minimum: " << minTemp << "°C on "
                 << std::put_time(localtime(&coldestTime), "%Y-%m-%d %H:%M") << "\n";
        
        std::cout << "\nOther Averages:\n";
        std::cout << "  Humidity: " << std::fixed << std::setprecision(0) 
                 << sumHum / readings.size() << "%\n";
        std::cout << "  Pressure: " << sumPress / readings.size() << " hPa\n";
        std::cout << "  Wind Speed: " << std::fixed << std::setprecision(1) 
                 << sumWind / readings.size() << " km/h\n";
        
        std::cout << "\nTemperature Distribution:\n";
        for (const auto& pair : tempByCategory) {
            if (countByCategory[pair.first] > 0) {
                double avg = pair.second / countByCategory[pair.first];
                double percent = (static_cast<double>(countByCategory[pair.first]) / 
                                readings.size()) * 100;
                std::cout << "  " << std::setw(10) << std::left << pair.first 
                         << ": " << std::fixed << std::setprecision(1) << avg << "°C"
                         << " (" << std::setprecision(0) << percent << "% of time)\n";
            }
        }
        
        std::cout << "\nWeather Conditions:\n";
        for (const auto& pair : conditionCount) {
            double percent = (static_cast<double>(pair.second) / readings.size()) * 100;
            std::cout << "  " << std::setw(10) << std::left << pair.first 
                     << ": " << pair.second << " times (" 
                     << std::fixed << std::setprecision(1) << percent << "%)\n";
        }
    }
    
    void displayTrends() {
        if (readings.size() < 48) { // Need at least 2 days of data
            std::cout << "Insufficient data for trend analysis.\n";
            return;
        }
        
        std::cout << "\n=== WEATHER TRENDS ===\n";
        
        // Calculate daily averages
        std::map<int, std::vector<double>> dailyTemps; // day index -> temperatures
        
        time_t startOfData = readings[0].timestamp;
        
        for (const auto& reading : readings) {
            int dayIndex = difftime(reading.timestamp, startOfData) / (24 * 3600);
            dailyTemps[dayIndex].push_back(reading.temperature);
        }
        
        std::cout << "Daily Temperature Trend:\n";
        for (const auto& pair : dailyTemps) {
            double sum = 0;
            for (double temp : pair.second) sum += temp;
            double avg = sum / pair.second.size();
            
            std::cout << "  Day " << pair.first + 1 << ": " 
                     << std::fixed << std::setprecision(1) << avg << "°C";
            
            // Trend indicator
            if (pair.first > 0) {
                double prevSum = 0;
                for (double temp : dailyTemps[pair.first - 1]) prevSum += temp;
                double prevAvg = prevSum / dailyTemps[pair.first - 1].size();
                
                if (avg > prevAvg + 1) std::cout << " ↗️";
                else if (avg < prevAvg - 1) std::cout << " ↘️";
                else std::cout << " →";
            }
            std::cout << "\n";
        }
        
        // Predict tomorrow's weather (simple average)
        int lastDay = dailyTemps.rbegin()->first;
        if (dailyTemps.find(lastDay - 1) != dailyTemps.end()) {
            double todaySum = 0, yesterdaySum = 0;
            for (double temp : dailyTemps[lastDay]) todaySum += temp;
            for (double temp : dailyTemps[lastDay - 1]) yesterdaySum += temp;
            
            double todayAvg = todaySum / dailyTemps[lastDay].size();
            double yesterdayAvg = yesterdaySum / dailyTemps[lastDay - 1].size();
            
            double predicted = todayAvg + (todayAvg - yesterdayAvg) * 0.5;
            
            std::cout << "\n📊 Prediction for tomorrow: " 
                     << std::fixed << std::setprecision(1) << predicted << "°C";
            
            if (predicted > todayAvg + 2) std::cout << " (Warmer)";
            else if (predicted < todayAvg - 2) std::cout << " (Cooler)";
            else std::cout << " (Similar)";
            std::cout << "\n";
        }
    }
    
    void findExtremes() {
        if (readings.empty()) return;
        
        std::cout << "\n=== WEATHER EXTREMES ===\n";
        
        WeatherData hottest = readings[0];
        WeatherData coldest = readings[0];
        WeatherData wettest = readings[0]; // highest humidity
        WeatherData windiest = readings[0];
        
        for (const auto& reading : readings) {
            if (reading.temperature > hottest.temperature) hottest = reading;
            if (reading.temperature < coldest.temperature) coldest = reading;
            if (reading.humidity > wettest.humidity) wettest = reading;
            if (reading.windSpeed > windiest.windSpeed) windiest = reading;
        }
        
        std::cout << "🌡️  Hottest: " << hottest.temperature << "°C on "
                 << std::put_time(localtime(&hottest.timestamp), "%Y-%m-%d %H:%M") 
                 << " (" << hottest.condition << ")\n";
        
        std::cout << "❄️  Coldest: " << coldest.temperature << "°C on "
                 << std::put_time(localtime(&coldest.timestamp), "%Y-%m-%d %H:%M") 
                 << " (" << coldest.condition << ")\n";
        
        std::cout << "💧 Wettest: " << wettest.humidity << "% humidity on "
                 << std::put_time(localtime(&wettest.timestamp), "%Y-%m-%d %H:%M") 
                 << " (" << wettest.condition << ")\n";
        
        std::cout << "💨 Windiest: " << windiest.windSpeed << " km/h on "
                 << std::put_time(localtime(&windiest.timestamp), "%Y-%m-%d %H:%M") 
                 << " (" << windiest.condition << ")\n";
    }
    
    void displayHeatIndexWarning() {
        std::cout << "\n=== HEAT INDEX WARNINGS ===\n";
        
        int warningCount = 0;
        for (const auto& reading : readings) {
            double heatIndex = reading.calculateHeatIndex();
            if (heatIndex > 30) {
                struct tm* timeinfo = localtime(&reading.timestamp);
                std::cout << "⚠️  " << std::put_time(timeinfo, "%Y-%m-%d %H:%M")
                         << ": Heat Index " << std::fixed << std::setprecision(1) 
                         << heatIndex << "°C - ";
                
                if (heatIndex > 40) std::cout << "DANGER - Heat stroke likely";
                else if (heatIndex > 35) std::cout << "EXTREME CAUTION - Heat stroke possible";
                else if (heatIndex > 30) std::cout << "CAUTION - Fatigue possible";
                
                std::cout << "\n";
                warningCount++;
                
                if (warningCount >= 5) break; // Limit output
            }
        }
        
        if (warningCount == 0) {
            std::cout << "✅ No dangerous heat conditions detected.\n";
        }
    }
};

int main() {
    WeatherStation station("New York");
    
    // Generate random data for 7 days
    station.generateRandomReadings(7);
    
    int choice;
    do {
        std::cout << "\n=== WEATHER DATA SIMULATOR ===\n";
        std::cout << "Location: New York\n";
        std::cout << "1. View Recent Readings\n";
        std::cout << "2. View Statistics\n";
        std::cout << "3. View Weather Trends\n";
        std::cout << "4. Find Weather Extremes\n";
        std::cout << "5. Heat Index Warnings\n";
        std::cout << "6. Generate New Data (7 days)\n";
        std::cout << "7. Exit\n";
        std::cout << "Choice: ";
        std::cin >> choice;
        
        if (choice == 1) {
            station.displayRecentReadings();
        } else if (choice == 2) {
            station.displayStatistics();
        } else if (choice == 3) {
            station.displayTrends();
        } else if (choice == 4) {
            station.findExtremes();
        } else if (choice == 5) {
            station.displayHeatIndexWarning();
        } else if (choice == 6) {
            station = WeatherStation("New York"); // Reset
            station.generateRandomReadings(7);
            std::cout << "✅ Generated 7 days of new weather data.\n";
        }
    } while (choice != 7);
    
    return 0;
}

