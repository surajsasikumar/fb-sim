#ifndef FBSIM_H
#define FBSIM_H

#include "FOOTBALL.H"
#include "POISSON.H"
#include <unistd.h>
#include <vector>
#include <limits>
#include <string>
#include <sstream>

using namespace std;

void statement(char com[500], int x);
void getSquad(int x, int y[11]);
void getName(int x, char y[18]);
void getPlayerName(int x, char p[30]);
void playMatch();
void typeDelay(char x[500], char y[50]);
float calculation(int x, int y[11]);
float aggregateCalculation(int s[11]);
int getPlayStat(int x, int y);
void foul(int x, int y, int z, int t1[11], int t2[11]);
void logUpdate(int g1, int g2);

extern int rep;

#define C_RESET  "\033[0m"
#define C_GOAL   "\033[1;33m"
#define C_TEAM1  "\033[1;32m"
#define C_TEAM2  "\033[1;34m"
#define C_INFO   "\033[36m"
#define C_DIM    "\033[2m"
#define C_OK     "\033[1;32m"


inline std::string minStr(int m){ std::ostringstream os; os<<m<<"’"; return os.str(); }

inline void scoreBar(const char* t1, const char* t2, int g1, int g2, int minute) {
    cout << C_INFO << "[ " << C_TEAM1 << t1 << C_INFO << " " << g1
         << "  –  " << g2 << " " << C_TEAM2 << t2 << C_INFO << "  |  "
         << minStr(minute) << " ]" << C_RESET << "\n";
}

inline void pattern(int x) {
    if (x == 0) {
        std::cout << C_GOAL << "\n★  G O A L !  ★" << C_RESET << "\n";
        for (int i = 0; i < 40; i++) { std::cout << (i%2 ? "*" : "·"); nap_ms(12); }
        std::cout << "\n";
    } else {
        std::cout << C_OK << "\n✔ RESULT" << C_RESET << "\n";
        for (int i = 0; i < 50; i++) { std::cout << "#"; nap_ms(8); }
        std::cout << "\n";
    }
}

inline int arrayCheck(int x[6][10], int m) {
    for (int i = 0; i < 6; i++) {
        for (int j = 0; j < 10; j++) {
            if (x[i][j] == m) return i;
        }
    }
    return -1;
}

inline void arraySet(int x[6][10]) {
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 10; j++)
            x[i][j] = -1;
}

inline void statCalculation(int x, int y, int z, int a[6][10]) {
    int i = 0;
    while (i < x + y) {
        int m = rand() % 90;
        if (arrayCheck(a, m) == -1) {
            a[z][i] = m;
            i++;
        }
    }
}

inline void fbSim(float as1, float as2, float ds1, float ds2, int g1, int g2, int arr[6][10]) {
    int r;
    arraySet(arr);

    int x = 2 + rand() % 2;
    if (as1 >= as2) {
        r = rand() % 3;
        statCalculation(x, r, 2, arr);
        statCalculation(x, 0, 3, arr);
    } else {
        r = rand() % 3;
        statCalculation(x, r, 3, arr);
        statCalculation(x, 0, 2, arr);
    }

    x = 2 + rand() % 2;
    if (ds1 >= ds2) {
        r = rand() % 3;
        statCalculation(x, r, 4, arr);
        statCalculation(x, 0, 5, arr);
    } else {
        r = rand() % 3;
        statCalculation(x, r, 5, arr);
        statCalculation(x, 0, 4, arr);
    }

    statCalculation(g1, 0, 0, arr);
    statCalculation(g2, 0, 1, arr);
}

inline void typeDelay(char x[500], char y[50]) {
    // scaled by SIM_SLOW
    int j = 1200 + rand() % 1800;
    for (int i = 0; x[i] != '\0'; i++) {
        if (x[i] == '%') std::cout << "\n";
        else if (x[i] == '$') std::cout << "\n\t\t\t\t\t";
        else std::cout << x[i];
        nap_us(j);
    }
    std::cout << y << "\n";
}

inline void comment(int x,int s1[11],int s2[11]) {
    int r,y; char com[500],pname[30];
    switch(x) {
        case 0: case 2: case 4: case 6: { // team1 events
            statement(com, x);
            r = (x==4||x==6) ? 6+rand()%3 : rand()%5; // line with role bias
            y = s1[r]; getPlayerName(y,pname);
            cout << ((x==0)?C_GOAL:C_TEAM1); typeDelay(com,pname); cout << C_RESET;
            if (x==0) pattern(0);
            break;
        }
        case 1: case 3: case 5: case 7: { // team2 events
            statement(com, x);
            r = (x==5||x==7) ? 6+rand()%3 : rand()%5;
            y = s2[r]; getPlayerName(y,pname);
            cout << ((x==1)?C_GOAL:C_TEAM2); typeDelay(com,pname); cout << C_RESET;
            if (x==1) pattern(0);
            break;
        }
        default:
            break;
    }
}

inline void match(float as1,float as2,float ds1,float ds2,int g1,int g2,float ag1,float ag2,int t1id,int t2id)
{
    int arr[6][10], s1[11], s2[11];
    char t1[18] = {0}, t2[18] = {0};

    getName(t1id, t1);
    getName(t2id, t2);
    getSquad(t1id, s1);
    getSquad(t2id, s2);

    fbSim(as1,as2,ds1,ds2,g1,g2,arr);

    std::cout << C_INFO << "MATCH BEGINS\n" << C_RESET;
    nap_ms(300);
    std::cout << C_DIM << std::string(60, '-') << "\n" << C_RESET;
    scoreBar(t1,t2,0,0,0);

    int live1=0, live2=0;

    for (int i=0;i<90;i++) {
        if (i==0) { std::cout << C_INFO << "\t\tKICK OFF\n" << C_RESET; i=1; }

        // small heartbeat so minutes "pass" even without events
        nap_ms(35);

        foul(static_cast<int>(ag1), static_cast<int>(ag2), i, s1, s2);

        if (i==44) {
            std::cout << C_DIM << std::string(60, '-') << "\n" << C_RESET;
            std::cout << C_INFO << std::setw(30) << "HALF TIME\n" << C_RESET;
            scoreBar(t1,t2,live1,live2,i);
            nap_ms(900);
        }

        if (i==89) {
            std::cout << C_DIM << std::string(60, '-') << "\n" << C_RESET;
            std::cout << C_INFO << std::setw(30) << "FULL TIME\n\n" << C_RESET;
            if      (g1>g2) { std::cout << C_TEAM1 << std::setw(20) << t1 << " WINS\n" << C_RESET; pattern(1); }
            else if (g2>g1) { std::cout << C_TEAM2 << std::setw(20) << t2 << " WINS\n" << C_RESET; pattern(1); }
            else            { std::cout << C_INFO  << std::setw(20) << "DRAW\n"    << C_RESET;     pattern(1); }
            std::cout << C_INFO << t1 << " : " << g1 << "  —  " << t2 << " : " << g2 << C_RESET << "\n";
            nap_ms(900);
            break;
        }

        int x = arrayCheck(arr,i);
        if (x!=-1) {
            std::cout << C_DIM << i << "’  " << C_RESET;
            comment(x,s1,s2);
            std::cout << "\n";

            // linger a bit after notable moments
            if (x==0) { live1++; scoreBar(t1,t2,live1,live2,i); nap_ms(800); }
            else if (x==1) { live2++; scoreBar(t1,t2,live1,live2,i); nap_ms(800); }
            else { nap_ms(350); }
        }
    }

    // log at end
    logUpdate(g1,g2);
}

inline void getName(int y, char x[18]) {
    user u1;
    ifstream ifile("user.dat", ios::in | ios::binary);
    while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
        if (y == u1.userid) {
            strncpy(x, u1.tname, 17);
            x[17] = '\0';
            return;
        }
    }
    strcpy(x, "Team");
}

inline void getSquad(int id, int arr[11]) {
    user u1;
    ifstream ifile("user.dat", ios::in | ios::binary);
    while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
        if (id == u1.userid) {
            for (int i = 0; i < 11; i++) arr[i] = u1.squad[i];
            return;
        }
    }
    for (int i = 0; i < 11; i++) arr[i] = 0;
}

inline void printUsersBrief(const std::vector<user>& users) {
    cout << "\nAVAILABLE TEAMS\n";
    cout << left << setw(8) << "UserID" << setw(14) << "Team" << setw(18) << "Manager" << "\n";
    cout << string(40, '-') << "\n";
    for (const auto& u : users) {
        cout << left << setw(8) << u.userid
             << setw(14) << u.tname
             << setw(18) << u.username << "\n";
    }
    cout << "\n";
}

inline bool loadAllUsers(std::vector<user>& out) {
    ifstream ifile("user.dat", ios::in | ios::binary);
    if (!ifile) {
        cout << "No user.dat found. Create users first.\n";
        return false;
    }
    user u;
    while (ifile.read(reinterpret_cast<char*>(&u), sizeof(u))) {
        out.push_back(u);
    }
    if (out.empty()) {
        cout << "No users in user.dat. Create users first.\n";
        return false;
    }
    return true;
}

inline const user* findUserById(const std::vector<user>& users, int id) {
    for (const auto& u : users) if (u.userid == id) return &u;
    return nullptr;
}

inline void clearBadInput() {
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

inline void playMatch() {
    std::vector<user> users;
    if (!loadAllUsers(users)) return;

    printUsersBrief(users);

    int t1 = 0, t2 = 0;
    const user* U1 = nullptr;
    const user* U2 = nullptr;

    while (true) {
        cout << "Enter YOUR userid: ";
        if (!(cin >> t1)) { clearBadInput(); continue; }
        U1 = findUserById(users, t1);
        if (!U1) { cout << "Invalid userid. Please choose from the list above.\n"; continue; }
        break;
    }

    printUsersBrief(users);

    while (true) {
        cout << "Enter OPPONENT userid: ";
        if (!(cin >> t2)) { clearBadInput(); continue; }
        if (t2 == t1) { cout << "Opponent id must be different from your id.\n"; continue; }
        U2 = findUserById(users, t2);
        if (!U2) { cout << "Invalid userid. Please choose from the list above.\n"; continue; }
        break;
    }

    float avg = aGoal;
    int s1[11], s2[11];
    getSquad(t1, s1);
    getSquad(t2, s2);

    float as1 = calculation(0, s1);
    float ds1 = calculation(1, s1);
    float as2 = calculation(0, s2);
    float ds2 = calculation(1, s2);

    int a[2];
    poisson(as1, as2, ds1, ds2, avg, a);
    int g1 = a[0];
    int g2 = a[1];

    float agg1 = aggregateCalculation(s1);
    float agg2 = aggregateCalculation(s2);

    match(as1, as2, ds1, ds2, g1, g2, agg1, agg2, t1, t2);
}

inline float calculation(int x, int arr[11]) {
    float p = 0.0f;
    float y = 0.0f;
    int j;
    switch (x) {
        case 0: // attack: first 6 players
            for (int i = 0; i < 6; i++) {
                j = arr[i];
                p += getPlayStat(0, j);
            }
            y = (p / 6.0f) / 80.0f;
            break;
        case 1: // defense: last 5
            for (int i = 6; i < 11; i++) {
                j = arr[i];
                p += getPlayStat(0, j);
            }
            y = (p / 5.0f) / 80.0f;
            break;
        default:
            break;
    }
    return y;
}

inline void statement(char com[500], int x) {
    const char* G[] = {
        "It’s in! A deadly finish by ",
        "Thunderous strike from ",
        "What a move — calmly slotted home by ",
        "A towering header from "
    };
    const char* A1[] = {
        "Silky one-two, delightful touch by ",
        "Jinks past two… cross whipped in by ",
        "Midfield maestro at work — vision from ",
        "Low fizzing cross — great delivery by "
    };
    const char* A2[] = {
        "Lightning break! Counter led by ",
        "Space opens up… clever feet from ",
        "Cuts inside and lets fly — just wide from ",
        "This pressure is building — strings pulled by "
    };
    const char* D1[] = {
        "Superb sliding challenge by ",
        "Reads it early — interception from ",
        "Brave block! Big moment for ",
        "Last-ditch tackle — rescued by "
    };
    const char* D2[] = {
        "Stands tall — calm defending from ",
        "Clear and composed — nice work by ",
        "Snuffs out the danger — excellent play from ",
        "Timed to perfection — tackle by "
    };
    const char* S1[] = {
        "Flag up — offside against ",
        "Foul given — dangerous free kick conceded by ",
        "Corner awarded — set-piece to be taken by ",
        "Quick throw-in — restarted by "
    };
    const char* S2[] = {
        "Handball appeals… referee points for a free kick — against ",
        "Wild clearance into the stands by ",
        "Cynical nudge — referee has a word with ",
        "Nasty clip — booking could be coming for "
    };

    auto pick = [&](const char** arr, int n){ return arr[rand()%n]; };

    switch (x) {
        case 0: strcpy(com, pick(G,4)); break;   // team1 goal
        case 1: strcpy(com, pick(G,4)); break;   // team2 goal
        case 2: strcpy(com, pick(A1,4)); break;  // team1 attack
        case 3: strcpy(com, pick(A2,4)); break;  // team2 attack
        case 4: strcpy(com, pick(D1,4)); break;  // team1 defend
        case 5: strcpy(com, pick(D2,4)); break;  // team2 defend
        case 6: strcpy(com, pick(S1,4)); break;  // set-piece 1
        case 7: strcpy(com, pick(S2,4)); break;  // set-piece 2
        default: break;
    }
}

inline float aggregateCalculation(int s[11]) {
    int m = 0;
    for (int i = 0; i < 11; i++) m += getPlayStat(1, s[i]);
    return static_cast<float>(m) / 10.0f;
}

inline int getPlayStat(int x, int y) {
    player p1; int m = 0;
    ifstream ifile("player.dat", ios::in | ios::binary);
    if (!ifile) return 0;
    if (x == 0) {
        while (ifile.read(reinterpret_cast<char*>(&p1), sizeof(p1))) {
            if (y == p1.playerid) { m = p1.prating; break; }
        }
    } else {
        while (ifile.read(reinterpret_cast<char*>(&p1), sizeof(p1))) {
            if (y == p1.playerid) { m = p1.paggression; break; }
        }
    }
    return m;
}

inline void foul(int x, int y, int z, int t1[11], int t2[11]) {
    if (z == 0) return;

    int flag = -1;
    if (z > 4) {
        if (rep != -1) {
            if (z < rep - 4 || z > rep + 4) flag = 0;
            else return;
        } else rep = z;
    }

    if (flag == 0) {
        int m = 35 + rand() % 18;
        if (m % 13 == 0) {
            if (x > y) { cout << z << "th min:"; comment(6, t1, t2); rep = z; }
            else       { cout << z << "th min:"; comment(7, t1, t2); rep = z; }
        } else if (m % 17 == 0) {
            int t = rand() % 2;
            if (t == 0) { cout << z << "th min:"; comment(6, t1, t2); rep = z; }
            else        { cout << z << "th min:"; comment(7, t1, t2); rep = z; }
        }
    }
}

inline void logUpdate(int g1, int g2) {
    ofstream ofile("mlog.dat", ios::app | ios::binary);
    int one = 1; ofile.write(reinterpret_cast<char*>(&one), sizeof(one));
    ofile.close();

    int tot = g1 + g2;
    ofile.open("glog.dat", ios::app | ios::binary);
    ofile.write(reinterpret_cast<char*>(&tot), sizeof(tot));
    ofile.close();
}

#endif