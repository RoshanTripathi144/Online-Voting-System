#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

using namespace std;

enum Citizenship { NaturalBorn, Naturalized };

// Base class for people (voter, candidate)
class Person {
protected:
    string name;
    int age;
    Citizenship citizenship;
    string voterID;

public:
    Person(string name, int age, Citizenship citizenship, string voterID) :
        name(name), age(age), citizenship(citizenship), voterID(voterID) {}

    virtual void showDetails() = 0; // Polymorphism

    string getName() { return name; }  // Getter for name
    string getVoterID() { return voterID; }  // Getter for voterID
    int getAge() { return age; }  // Getter for age
};

// Candidate class inheriting from Person
class Candidate : public Person {
protected:
    string politicalParty;
    string role; // President or Vice President
    int voteCount;
    static map<string, Candidate*> vicePresidents; // Static map to keep track of all Vice Presidents

public:
    Candidate(string name, int age, Citizenship citizenship, string voterID, string party, string role) :
        Person(name, age, citizenship, voterID), politicalParty(party), role(role), voteCount(0) {}

    void castVote() {
        voteCount++;
        // Update all Vice Presidents if the candidate is a President
        if (role == "President") {
            for (auto& vicePresidentPair : vicePresidents) {
                if (vicePresidentPair.second->getParty() == politicalParty) {
                    vicePresidentPair.second->setVoteCount(voteCount); // Sync vote count with President
                }
            }
        } else if (role == "Vice President") {
            // Ensure Vice President's vote count is synchronized with the President's
            for (auto& presidentPair : Candidate::vicePresidents) {
                if (presidentPair.second->getParty() == politicalParty) {
                    setVoteCount(presidentPair.second->getVoteCount());
                    break;
                }
            }
        }
    }

    void showResults() {
        cout << "Name: " << name << ", Party: " << politicalParty
             << ", Role: " << role << ", Votes: " << voteCount << endl;
    }

    int getVoteCount() { return voteCount; }

    void setVoteCount(int count) { voteCount = count; }

    virtual void showDetails() override {
        cout << "Name: " << name << ", Age: " << age << ", Party: " << politicalParty
             << ", Role: " << role << endl;
    }

    string getParty() { return politicalParty; }

    static void addVicePresident(Candidate* vicePresident) {
        vicePresidents[vicePresident->getVoterID()] = vicePresident;
    }
};

// Initialize static member
map<string, Candidate*> Candidate::vicePresidents;

// Voter class inheriting from Person
class Voter : public Person {
private:
    bool hasVoted;

public:
    Voter(string name, int age, Citizenship citizenship, string voterID) :
        Person(name, age, citizenship, voterID), hasVoted(false) {}

    bool checkEligibility() {
        return age >= 18;  // Check if voter is eligible
    }

    void vote() { hasVoted = true; }

    bool hasAlreadyVoted() { return hasVoted; }

    virtual void showDetails() override {
        cout << "Voter Name: " << name << ", Voter ID: " << voterID << ", Age: " << age << endl;
    }

    void resetVotingStatus() { hasVoted = false; }  // Reset voting status for the voter
};

// Admin class to manage the election
class Admin {
private:
    string username = "admin";
    string password = "admin123";
    vector<Voter*> voters;
    map<string, Candidate*> presidentCandidates;
    map<string, Candidate*> vicePresidentCandidates;
    map<string, bool> partyNominated;
    map<string, int> votes;

    // Temporary storage for clearing
    vector<Voter*> tempVoters;
    map<string, Candidate*> tempPresidentCandidates;
    map<string, Candidate*> tempVicePresidentCandidates;

public:
    // Admin Login
    bool login(string uname, string pass) {
        return uname == username && pass == password;
    }

    // Change password
    void changePassword(string newPassword) {
        password = newPassword;
    }

    // Register President
    void registerPresident() {
        string name, voterID, party;
        int age;
        Citizenship citizenship;

        cout << "Enter President's name: ";
        getline(cin >> ws, name);
        cout << "Enter President's age: ";
        cin >> age;
        cout << "Enter President's citizenship (0 for Natural-Born, 1 for Naturalized): ";
        int cit;
        cin >> cit;
        citizenship = static_cast<Citizenship>(cit);
        cout << "Enter President's voter ID: ";
        cin >> voterID;
        cout << "Enter President's political party: ";
        cin >> party;

        if (age < 35 || citizenship == Naturalized) {
            cout << "Invalid candidate. Must be at least 35 years old and a natural-born citizen.\n";
            return;
        }

        if (partyNominated[party]) {
            cout << "Candidate from this party is already nominated!" << endl;
            return;
        }

        Candidate* president = new Candidate(name, age, citizenship, voterID, party, "President");
        presidentCandidates[voterID] = president;
        partyNominated[party] = true;

        // Sync Vice President votes with President votes
        for (auto& candidatePair : vicePresidentCandidates) {
            if (candidatePair.second->getParty() == party) {
                candidatePair.second->setVoteCount(president->getVoteCount());  // Sync vote count with President
            }
        }

        cout << "President " << name << " from " << party << " registered successfully!" << endl;
    }

    // Register Vice President
    void registerVicePresident() {
        string name, voterID, party;
        int age;
        Citizenship citizenship;

        cout << "Enter Vice President's name: ";
        getline(cin >> ws, name);
        cout << "Enter Vice President's age: ";
        cin >> age;
        cout << "Enter Vice President's citizenship (0 for Natural-Born, 1 for Naturalized): ";
        int cit;
        cin >> cit;
        citizenship = static_cast<Citizenship>(cit);
        cout << "Enter Vice President's voter ID: ";
        cin >> voterID;
        cout << "Enter Vice President's political party: ";
        cin >> party;

        if (age < 35 || citizenship == Naturalized) {
            cout << "Invalid candidate. Must be at least 35 years old and a natural-born citizen.\n";
            return;
        }

        if (presidentCandidates.empty()) {
            cout << "No President candidates registered yet!" << endl;
            return;
        }

        bool foundParty = false;
        for (auto& candidatePair : presidentCandidates) {
            if (candidatePair.second->getParty() == party) {
                foundParty = true;
                break;
            }
        }

        if (!foundParty) {
            cout << "No President candidate found from this party!" << endl;
            return;
        }

        if (vicePresidentCandidates.find(voterID) != vicePresidentCandidates.end()) {
            cout << "Vice President from this party is already nominated!" << endl;
            return;
        }

        Candidate* vicePresident = new Candidate(name, age, citizenship, voterID, party, "Vice President");
        vicePresidentCandidates[voterID] = vicePresident;

        // Sync the vote count with the President if it exists
        for (auto& candidatePair : presidentCandidates) {
            if (candidatePair.second->getParty() == party) {
                vicePresident->setVoteCount(candidatePair.second->getVoteCount());  // Sync vote count with President
                break;
            }
        }

        Candidate::addVicePresident(vicePresident);

        cout << "Vice President " << name << " from " << party << " registered successfully!" << endl;
    }

    // Register Voter
    void registerVoter() {
        string name, voterID;
        int age;
        Citizenship citizenship;

        cout << "Enter voter's name: ";
        getline(cin >> ws, name);
        cout << "Enter voter's age: ";
        cin >> age;
        cout << "Enter voter's citizenship (0 for Natural-Born, 1 for Naturalized): ";
        int cit;
        cin >> cit;
        citizenship = static_cast<Citizenship>(cit);
        cout << "Enter voter's voter ID: ";
        cin >> voterID;

        if (age < 18) {
            cout << "Invalid voter. Must be at least 18 years old.\n";
            return;
        }

        Voter* voter = new Voter(name, age, citizenship, voterID);
        voters.push_back(voter);
        cout << "Voter " << name << " registered successfully!" << endl;
    }

    // Vote process
    void vote() {
        string voterID, party;

        cout << "Enter Voter ID: ";
        cin >> voterID;
        cout << "Enter Candidate's Political Party: ";
        cin >> party;

        bool validVote = false;
        for (auto& voter : voters) {
            if (voter->getVoterID() == voterID && !voter->hasAlreadyVoted()) {
                if (voter->checkEligibility()) {
                    voter->vote();
                    // Cast vote for the President
                    for (auto& candidatePair : presidentCandidates) {
                        if (candidatePair.second->getParty() == party) {
                            candidatePair.second->castVote();
                            validVote = true;
                            break;
                        }
                    }
                }
                break;
            }
        }

        if (validVote) {
            cout << "Vote cast successfully!" << endl;
        } else {
            cout << "Invalid voter ID or voter has already voted or no candidate from this party!" << endl;
        }
    }

    // View Voter List
    void viewVoterList() {
        for (auto& voter : voters) {
            voter->showDetails();
        }
    }

    // Check Candidate Details
    void checkCandidateDetails() {
        cout << "Presidential Candidates:\n";
        for (auto& candidatePair : presidentCandidates) {
            candidatePair.second->showDetails();
        }

        cout << "\nVice Presidential Candidates:\n";
        for (auto& candidatePair : vicePresidentCandidates) {
            candidatePair.second->showDetails();
        }
    }

    // Display Results
    void displayResults() {
        cout << "Presidential Candidates:\n";
        for (auto& candidatePair : presidentCandidates) {
            candidatePair.second->showResults();
        }

        cout << "\nVice Presidential Candidates:\n";
        for (auto& candidatePair : vicePresidentCandidates) {
            candidatePair.second->showResults();
        }
    }

    // Save Results to File
    void saveResultsToFile() {
        ofstream file("results.txt");

        file << "Presidential Candidates:\n";
        for (auto& candidatePair : presidentCandidates) {
            file << "Name: " << candidatePair.second->getName() << ", Party: " << candidatePair.second->getParty()
                 << ", Votes: " << candidatePair.second->getVoteCount() << "\n";
        }

        file << "\nVice Presidential Candidates:\n";
        for (auto& candidatePair : vicePresidentCandidates) {
            file << "Name: " << candidatePair.second->getName() << ", Party: " << candidatePair.second->getParty()
                 << ", Votes: " << candidatePair.second->getVoteCount() << "\n";
        }

        file.close();
        cout << "Results saved to results.txt" << endl;
    }

    // Clear All Votes
    void clearVotes() {
        for (auto& candidatePair : presidentCandidates) {
            candidatePair.second->setVoteCount(0);
        }
        for (auto& candidatePair : vicePresidentCandidates) {
            candidatePair.second->setVoteCount(0);
        }
        cout << "Votes cleared!" << endl;
    }

    // Clear All Candidates
    void clearCandidates() {
        // Save candidate details before clearing
        tempPresidentCandidates = presidentCandidates;
        tempVicePresidentCandidates = vicePresidentCandidates;

        // Delete all Presidential Candidates
        for (auto& candidatePair : presidentCandidates) {
            delete candidatePair.second;  // Free memory allocated for the candidate
        }
        presidentCandidates.clear();  // Clear the map

        // Delete all Vice Presidential Candidates
        for (auto& candidatePair : vicePresidentCandidates) {
            delete candidatePair.second;  // Free memory allocated for the candidate
        }
        vicePresidentCandidates.clear();  // Clear the map

        partyNominated.clear();  // Clear party nominations

        cout << "Candidates cleared!" << endl;
    }

    // Clear All Voters
    void clearVoters() {
        // Save voter details before clearing
        tempVoters = voters;

        // Delete all Voters
        for (auto& voter : voters) {
            delete voter;  // Free memory allocated for the voter
        }
        voters.clear();  // Clear the vector

        cout << "Voters cleared!" << endl;
    }
};

int main() {
    Admin admin;
    string username, password;
    int choice;

    cout << "Enter Admin Username: ";
    cin >> username;
    cout << "Enter Admin Password: ";
    cin >> password;

    if (!admin.login(username, password)) {
        cout << "Invalid credentials!" << endl;
        return 1;
    }

    while (true) {
        cout << "\nAdmin Menu:\n";
        cout << "1. Register President\n";
        cout << "2. Register Vice President\n";
        cout << "3. Register Voter\n";
        cout << "4. Vote\n";
        cout << "5. View Voter List\n";
        cout << "6. Check Candidate Details\n";
        cout << "7. Display Results\n";
        cout << "8. Save Results to File\n";
        cout << "9. Clear All Votes\n";
        cout << "10. Clear All Candidates\n";
        cout << "12. Clear All Voters\n";
        cout << "13. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                admin.registerPresident();
                break;
            case 2:
                admin.registerVicePresident();
                break;
            case 3:
                admin.registerVoter();
                break;
            case 4:
                admin.vote();
                break;
            case 5:
                admin.viewVoterList();
                break;
            case 6:
                admin.checkCandidateDetails();
                break;
            case 7:
                admin.displayResults();
                break;
            case 8:
                admin.saveResultsToFile();
                break;
            case 9:
                admin.clearVotes();
                break;
            case 10:
                admin.clearCandidates();
                break;
            case 12:
                admin.clearVoters();
                break;

            case 13:
                return 0;
            default:
                cout << "Invalid choice! Try again." << endl;
                break;
        }
    }

    return 0;
}
