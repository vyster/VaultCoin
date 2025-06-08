#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

struct Team {
    string name;
    int played = 0, won = 0, nr = 0, points = 0;
};

map<string, Team> initPointsTable() {
    map<string, Team> table;
    vector<tuple<string, int, int, int, int>> data = {
        {"RCB", 11, 8, 0, 16}, {"PBKS", 11, 7, 1, 15}, {"MI", 11, 7, 0, 14},
        {"GT", 10, 7, 0, 14}, {"DC", 10, 6, 0, 12}, {"KKR", 11, 5, 1, 11},
        {"LSG", 11, 5, 0, 10}, {"RR", 12, 3, 0, 6}, {"SRH", 10, 3, 0, 6}, {"CSK", 11, 2, 0, 4}
    };
    for (int i = 0; i < data.size(); ++i) {
        table[get<0>(data[i])] = {get<0>(data[i]), get<1>(data[i]), get<2>(data[i]), get<3>(data[i]), get<4>(data[i])};
    }
    return table;
}

vector<pair<string, string>> getUpcomingMatches() {
    return {
        {"SRH", "DC"}, {"MI", "GT"}, {"KKR", "CSK"}, {"PBKS", "DC"},
        {"LSG", "RCB"}, {"SRH", "KKR"}, {"PBKS", "MI"}, {"DC", "GT"},
        {"CSK", "RR"}, {"RCB", "SRH"}, {"GT", "LSG"}, {"MI", "DC"},
        {"RR", "PBKS"}, {"RCB", "KKR"}, {"GT", "CSK"}, {"LSG", "SRH"}
    };
}

void simulatePrediction(const vector<string>& predictedWinners, const string& focusTeam) {
    auto table = initPointsTable();
    auto matches = getUpcomingMatches();
    for (int i = 0; i < predictedWinners.size(); ++i) {
        string winner = predictedWinners[i];
        string loser = (matches[i].first == winner) ? matches[i].second : matches[i].first;
        table[winner].won += 1;
        table[winner].points += 2;
        table[winner].played += 1;
        table[loser].played += 1;
    }
    
    vector<Team> sorted;
    for (map<string, Team>::iterator it = table.begin(); it != table.end(); ++it) {
        sorted.push_back(it->second);
    }
    sort(sorted.begin(), sorted.end(), [](Team &a, Team &b) {
        if (a.points != b.points) return a.points > b.points;
        return a.name < b.name;
    });

    cout << "\nFinal Points Table:\n";
    for (auto &t : sorted) {
        cout << t.name << "\t" << t.played << "\t" << t.points << endl;
    }
    
    bool qualified = false;
    for (int i = 0; i < 4; ++i) {
        if (sorted[i].name == focusTeam) qualified = true;
    }
    cout << "\n" << focusTeam << (qualified ? " QUALIFIES" : " DOES NOT QUALIFY") << " for playoffs.\n";
}

double simulateAllScenarios(const string& focusTeam) {
    auto matches = getUpcomingMatches();
    int total = 1 << matches.size();  // 2^16 = 65536 scenarios
    int qualifiedCount = 0;

    for (int mask = 0; mask < total; ++mask) {
        auto table = initPointsTable();
        for (int i = 0; i < matches.size(); ++i) {
            string winner = (mask & (1 << i)) ? matches[i].first : matches[i].second;
            string loser = (matches[i].first == winner) ? matches[i].second : matches[i].first;
            table[winner].won++;
            table[winner].points += 2;
            table[winner].played++;
            table[loser].played++;
        }
        vector<Team> sorted;
        for (map<string, Team>::iterator it = table.begin(); it != table.end(); ++it) {
            sorted.push_back(it->second);
        }
        sort(sorted.begin(), sorted.end(), [](Team &a, Team &b) {
            if (a.points != b.points) return a.points > b.points;
            return a.name < b.name;
        });
        for (int i = 0; i < 4; ++i) {
            if (sorted[i].name == focusTeam) {
                qualifiedCount++;
                break;
            }
        }
    }
    double percent = (double)qualifiedCount / total * 100.0;
    return percent;
}

int main() {
    vector<pair<string, string>> matches = getUpcomingMatches();
    vector<string> prediction;
    vector<string> allTeams = {
        "RCB", "PBKS", "MI", "GT", "DC", "KKR", "LSG", "RR", "SRH", "CSK"
    };

    cout << "Qualification Chances for Each Team (%):\n";
    cout << "----------------------------------------\n";

    for (int i = 0; i < allTeams.size(); ++i) {
        double chance = simulateAllScenarios(allTeams[i]);
        cout << allTeams[i] << " : " << chance << "%" << endl;
    }
    cout << "Please enter your predictions for the remaining matches (team names only):\n";
    for (int i = 0; i < matches.size(); ++i) {
        string winner;
        cout << "Match " << i + 1 << ": " << matches[i].first << " vs " << matches[i].second << "\n";
        cout << "Enter predicted winner: ";
        cin >> winner;
        prediction.push_back(winner);
    }

    string focusTeam;
    cout << "\nEnter the team you want to check for qualification: ";
    cin >> focusTeam;

    // Simulate prediction and print results
    simulatePrediction(prediction, focusTeam);

    return 0;
}

