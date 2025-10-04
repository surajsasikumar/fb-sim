#ifndef FOOTBALL_H
#define FOOTBALL_H

#include <thread>
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <cctype>
#include <unistd.h>
#include <limits>
#include <string>
#include <vector>
#include <chrono>

using namespace std;

void displayLog(int e);
int idCheck(int x, int y);
int team(int i, int *amount, int level);
int buildSquad(int *amount, int level, char position[10]);
int buyPlayer(int *amount);
void playMatch();

#define CLR_RESET  "\033[0m"
#define CLR_DIM    "\033[2m"
#define CLR_BOLD   "\033[1m"
#define CLR_TITLE  "\033[1;36m"
#define CLR_MENU   "\033[38;5;246m"
#define CLR_OK     "\033[1;32m"
#define CLR_WARN   "\033[1;33m"
#define CLR_ERR    "\033[1;31m"

extern float aGoal;

inline bool safeEquals(const std::string& a, const std::string& b) {
    if (a.size() != b.size()) return false;
    unsigned char r = 0;
    for (size_t i = 0; i < a.size(); ++i) r |= static_cast<unsigned char>(a[i] ^ b[i]);
    return r == 0;
}

#ifdef _WIN32
  #include <windows.h>
  inline std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
    while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r' || s[b-1] == '\n')) --b;
    return s.substr(a, b - a);
  }

  inline std::string envOrEmpty(const char* name) {
    const char* v = std::getenv(name);
    return v ? trim(std::string(v)) : std::string();
  }

  inline std::string readHidden(const char* prompt) {
        std::cout << prompt;
        HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
        DWORD oldMode = 0; GetConsoleMode(hIn, &oldMode);
        SetConsoleMode(hIn, oldMode & ~ENABLE_ECHO_INPUT);
        std::string s; std::getline(std::cin >> std::ws, s);
        SetConsoleMode(hIn, oldMode);
        std::cout << "\n";
        return trim(s);
  }

  extern double simSlow ;
  inline void nap_us(int us) { ::Sleep((us + 999) * simSlow / 1000); }
  inline void nap_ms(int ms) { ::Sleep(ms * simSlow); }
#else
  #include <termios.h>
  #include <unistd.h>
  // read a line with echo disabled (POSIX: macOS/Linux)
  inline std::string readHidden(const char* prompt) {
      std::cout << prompt;
      termios oldt{}; tcgetattr(STDIN_FILENO, &oldt);
      termios newt = oldt; newt.c_lflag &= ~ECHO;
      tcsetattr(STDIN_FILENO, TCSANOW, &newt);
      std::string s; std::getline(std::cin >> std::ws, s);
      tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
      std::cout << "\n";
      return s;
  }
  inline std::string envOrEmpty(const char* name) {
      const char* v = std::getenv(name);
      return v ? std::string(v) : std::string();
  }
  inline void nap_us(int us) { std::this_thread::sleep_for(std::chrono::microseconds(us)); }
  inline void nap_ms(int us) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }
#endif

inline bool gateWithEnvSecret(const char* envName, const char* roleLabel) {
      const std::string expected = envOrEmpty(envName); // now trimmed
      if (expected.empty()) {
          std::cout << "[WARN] " << roleLabel << " access disabled (set " << envName << ").\n";
          return false;
      }
      for (int tries = 0; tries < 3; ++tries) {
          std::string pass = readHidden((std::string(roleLabel) + " password: ").c_str()); // now trimmed
          if (safeEquals(pass, expected)) return true;
          std::cout << "Incorrect " << roleLabel << " password.\n";
          nap_ms(400);
      }
      std::cout << "Too many attempts.\n";
      return false;
}

inline void hr(int w=60, char c='-') {
    cout << string(w, c) << "\n";
}

inline void printMenuBox(const string& title, const vector<string>& items, bool showBack=true) {
    cout << "\n" << CLR_TITLE << "╔ " << title << " ╗" << CLR_RESET << "\n";
    hr( max<int>(static_cast<int>(title.size())+4, 30), '=');
    for (size_t i=0;i<items.size();++i) {
        cout << CLR_MENU << setw(2) << (i+1) << CLR_RESET << ") " << items[i] << "\n";
    }
    if (showBack) cout << CLR_MENU << " 0" << CLR_RESET << ") Back\n";
    hr();
}

inline int readMenuChoice() {
    string tok; cout << CLR_DIM << "Select an option: " << CLR_RESET;
    cin >> tok;
    if (tok=="b"||tok=="B") return 0;
    return atoi(tok.c_str());
}

inline int iCompare(const char* a, const char* b) {
    while (*a && *b) {
        int da = tolower(static_cast<unsigned char>(*a));
        int db = tolower(static_cast<unsigned char>(*b));
        if (da != db) return (da < db) ? -1 : 1;
        ++a; ++b;
    }
    if (*a == *b) return 0;
    return *a ? 1 : -1;
}

inline bool askYesNo(const char* prompt) {
    while (true) {
        cout << prompt;
        string s;
        if (!(cin >> s)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        if (s == "y" || s == "Y") return true;
        if (s == "n" || s == "N") return false;
        cout << "Please type y or n.\n";
    }
}

class player {
public:
    int playerid = 0, prating = 0, paggression = 0, price = 0;
    char playername[20] = {0}, category[10] = {0}, position[10] = {0};

    void getData();
    void display() const {
        char name[sizeof(playername)];  strncpy(name,  playername, sizeof(name)-1);  name[sizeof(name)-1]  = '\0';
        char cat [sizeof(category)];    strncpy(cat,   category,   sizeof(cat)-1);   cat[sizeof(cat)-1]   = '\0';
        char pos [sizeof(position)];    strncpy(pos,   position,   sizeof(pos)-1);   pos[sizeof(pos)-1]   = '\0';

        cout << left
             << setw(16) << name
             << setw(6)  << prating
             << setw(8)  << playerid
             << setw(8)  << price
             << setw(8)  << cat
             << setw(6)  << pos
             << "\n";
    }
};

#pragma pack(push,1)
struct PlayerRecord {
    int32_t playerid;
    int32_t prating;
    int32_t paggression;
    int32_t price;
    char    playername[20];
    char    category[10];
    char    position[10];
};
#pragma pack(pop)
static_assert(sizeof(PlayerRecord) == 4*4 + 20 + 10 + 10, "PlayerRecord size mismatch");

inline void toRecord(const player& p, PlayerRecord& r) {
    r.playerid    = p.playerid;
    r.prating     = p.prating;
    r.paggression = p.paggression;
    r.price       = p.price;
    auto clampcpy = [](char* dst, size_t cap, const char* src){
        size_t n = 0;
        while (n < cap - 1 && src[n] != '\0') ++n;
        std::memcpy(dst, src, n);
        dst[n] = '\0';
    };
    clampcpy(r.playername, sizeof(r.playername), p.playername);
    clampcpy(r.category,   sizeof(r.category),   p.category);
    clampcpy(r.position,   sizeof(r.position),   p.position);
}

inline void fromRecord(const PlayerRecord& r, player& p) {
    p.playerid    = r.playerid;
    p.prating     = r.prating;
    p.paggression = r.paggression;
    p.price       = r.price;
    memcpy(p.playername, r.playername, sizeof(p.playername));
    p.playername[sizeof(p.playername)-1] = '\0';
    memcpy(p.category,   r.category,   sizeof(p.category));
    p.category[sizeof(p.category)-1]     = '\0';
    memcpy(p.position,   r.position,   sizeof(p.position));
    p.position[sizeof(p.position)-1]     = '\0';
}

inline bool appendPlayer(const player& p) {
    PlayerRecord r{}; toRecord(p, r);
    ofstream of("player.dat", ios::binary | ios::app);
    if (!of) return false;
    of.write(reinterpret_cast<const char*>(&r), sizeof(r));
    return static_cast<bool>(of);
}

inline bool readPlayer(ifstream& in, player& out) {
    PlayerRecord r{};
    if (!in.read(reinterpret_cast<char*>(&r), sizeof(r))) return false;
    fromRecord(r, out);
    return true;
}


inline void printPlayerTableHeader() {
    cout << left
         << setw(16) << "Name"
         << setw(6)  << "Rate"
         << setw(8)  << "ID"
         << setw(8)  << "Price"
         << setw(8)  << "Cat"
         << setw(6)  << "Pos" << "\n";
    cout << string(52, '-') << "\n";
}

inline void player::getData() {
    int x;
    while (true) {
        cout << "enter player id: ";
        if (!(cin >> x)) {
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cout << "Please enter a numeric id.\n";
            continue;
        }
        if (x <= 0) {
            cout << "Please enter a positive id.\n";
            continue;
        }
        if (idCheck(1, x) == 0) {
            cout << "used by another player. please enter new playerid\n";
            continue;
        }
        break;
    }
    playerid = x;

    cout << "Enter the player rating: ";
    while (!(cin >> prating)) {
        cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Enter a number for rating: ";
    }
    cout << "Enter player aggression rate: ";
    while (!(cin >> paggression)) {
        cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Enter a number for aggression: ";
    }

    cout << "Enter player name: ";
    cin >> playername;

    cout << "Enter the category (elite/gold/silver): ";
    cin >> category;

    cout << "Enter position (fwd/mid/def/gk): ";
    cin >> position;

    cout << "Enter price of the player: ";
    while (!(cin >> price)) {
        cin.clear(); cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "Enter a number for price: ";
    }
}

class user {
public:
    int userid = 0, level = 0;
    char username[30] = {0};
    char tname[10] = {0};
    int squad[11] = {0};
    int amount = 0;

    void getData();
    void display() const { cout << userid << "\t\t" << tname << "\n"; }
};

inline void getPlayerName(int id, char out[30]) {
    ifstream ifile("player.dat", ios::in | ios::binary);
    if (!ifile) { cerr << "[WARN] Could not open player.dat. Returning Unknown for player " << id << "\n"; strcpy(out, "Unknown"); return; }
    while (true) {
        player tmp;
        if (!readPlayer(ifile, tmp)) break;
        if (id == tmp.playerid) { strncpy(out, tmp.playername, 29); out[29]='\0'; return; }
    }
    strcpy(out, "Unknown");
}

inline void user::getData() {
    int x;
    cout << "enter userid: ";
    cin >> x;
    while (idCheck(0, x) == 0) {
        cout << "used by another account..please enter new user id\n";
        cin >> x;
    }
    userid = x;
    cout << "Enter the user name: ";  cin >> username;
    cout << "Enter the team name: ";  cin >> tname;

    char ch;
    cout << "are you vip user (y/n): ";
    cin >> ch;
    if (ch == 'y' || ch == 'Y') {
        const std::string expected = envOrEmpty("FBSIM_VIP_PASS");
        if (expected.empty()) {
            cout << "[WARN] VIP mode disabled (set FBSIM_VIP_PASS to enable).\n";
            level = 5; amount = 15000;
        } else {
            bool ok = false;
            for (int tries=0; tries<3; ++tries) {
                std::string pass = readHidden("enter VIP pass: ");
                if (safeEquals(pass, expected)) { ok = true; break; }
                cout << "Incorrect VIP pass.\n";
                nap_ms(300);
            }
            if (ok) { level = 10; amount = 30000; }
            else    { level = 5;  amount = 15000; }
        }
    } else {
        level = 5; amount = 15000;
    }

    cout << "now you have to purchase players\n";
    cout << "amount available is >> " << amount << "\n";

    int i = 0;
    while (i < 11) {
        int pid = team(i, &amount, level);
        if (pid == -2) { // user chose Back inside buildSquad
            if (askYesNo("Cancel creating this user and go back? (y/n): ")) {
                userid = 0; // signal iUser() to NOT save this user
                return;
            } else {
                continue; // keep trying to fill this slot
            }
        }
        if (pid != -1) {
            cout << "you have to purchase " << (10 - i) << " players\n";
            squad[i] = pid;
            ++i;
        }
    }
}

inline void userDefine(int uid) {
    cout << "1.Display squad\n";
    int m; cin >> m;
    if (m != 1) return;

    ifstream ifile("user.dat", ios::in | ios::binary);
    user u1;
    while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
        if (uid == u1.userid) {
            cout << "ATTACKERS\n";
            for (int i = 0; i < 3; ++i) { char p[30]; getPlayerName(u1.squad[i], p); cout << p << "\t"; }
            cout << "\nMIDFIELDERS\n";
            for (int i = 3; i < 6; ++i) { char p[30]; getPlayerName(u1.squad[i], p); cout << p << "\t"; }
            cout << "\nDEFENDERS\n";
            for (int i = 6; i < 10; ++i){ char p[30]; getPlayerName(u1.squad[i], p); cout << p << "\t"; }
            cout << "\nGOALKEEPER\n";
            { char p[30]; getPlayerName(u1.squad[10], p); cout << p << "\n"; }
            return;
        }
    }
    cout << "User not found.\n";
}

inline void iUser() {
    while (true) {
        printMenuBox("User Menu", {"Create", "Login"});
        int ch = readMenuChoice();
        if (ch==0) return;

        switch (ch) {
            case 1: {
                ofstream ofile("user.dat", ios::app | ios::binary);
                if (!ofile) { cout << CLR_ERR << "The file cannot be opened\n" << CLR_RESET; exit(0); }
                do {
                    user u1;
                    u1.getData(); // may be canceled by Back inside squad flow
                    if (u1.userid > 0) {
                        ofile.write(reinterpret_cast<char*>(&u1), sizeof(u1));
                        cout << CLR_OK << "User saved.\n" << CLR_RESET;
                    } else {
                        cout << CLR_WARN << "User creation canceled.\n" << CLR_RESET;
                    }
                } while (askYesNo("Create another user? (y/n): "));
                break;
            }
            case 2: {
                cout << "Enter the user id to login: ";
                int usid; cin >> usid;
                ifstream ifile("user.dat", ios::in | ios::binary);
                if (!ifile) { cout << CLR_ERR << "The file cannot be opened\n" << CLR_RESET; break; }
                user u1; bool found=false;
                while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
                    if (usid == u1.userid) {
                        cout << CLR_OK << "WELCOME BACK " << u1.username << "\n" << CLR_RESET;
                        found = true; userDefine(usid); break;
                    }
                }
                if (!found) cout << CLR_WARN << "Account not found.\n" << CLR_RESET;
                break;
            }
            default: cout << CLR_WARN << "Invalid choice.\n" << CLR_RESET;
        }
    }
}

inline void admin() {
    if (!gateWithEnvSecret("FBSIM_ADMIN_PASS", "Admin")) return;

    player p1;

    while (true) {
        printMenuBox("Admin Menu", {"Insert player data", "Display player data", "Enter average goal", "Display log"});
        int ch = readMenuChoice();
        if (ch==0) return;

        switch (ch) {
            case 1: {
                cout << CLR_DIM << "Please enter position as fwd/mid/def/gk\n" << CLR_RESET;
                do {
                    p1.getData();
                    if (!appendPlayer(p1)) { cout << CLR_ERR << "The file cannot be opened\n" << CLR_RESET; break; }
                } while (askYesNo("Player added. Add another? (y/n): "));
                break;
            }
            case 2: {
                ifstream ifile("player.dat", ios::in | ios::binary);
                if (!ifile) { cout << CLR_ERR << "The file cannot be opened\n" << CLR_RESET; break; }
                printPlayerTableHeader();
                size_t shown=0; player ptmp;
                while (readPlayer(ifile, ptmp)) {
                    if (ptmp.playerid <= 0 || ptmp.playername[0] == '\0') continue;
                    ptmp.display(); ++shown;
                }
                if (shown==0) cout << CLR_WARN << "(no players found)\n" << CLR_RESET;
                break;
            }
            case 3: {
                printMenuBox("Average Goals", {"Manually enter avg goal", "Set avg goal from logs"});
                int s = readMenuChoice();
                if (s==1) {
                    cout << "enter average goal: "; cin >> aGoal;
                } else if (s==2) {
                    ifstream mlog("mlog.dat", ios::in | ios::binary);
                    ifstream glog("glog.dat", ios::in | ios::binary);
                    int log=0, matches=0, goals=0;
                    while (mlog.read(reinterpret_cast<char*>(&log), sizeof(log))) matches += 1;
                    while (glog.read(reinterpret_cast<char*>(&log), sizeof(log))) goals += log;
                    cout << "total matches " << matches << "\n";
                    cout << "total goals   " << goals << "\n";
                    if (matches>0) { aGoal = static_cast<float>(goals)/static_cast<float>(matches); cout << "average goal " << aGoal << "\n"; }
                }
                break;
            }
            case 4: {
                printMenuBox("Logs", {"User log", "Player log", "Match log", "Total goals"});
                int e = readMenuChoice();
                if (e==0) break;
                displayLog(e);
                break;
            }
            default: cout << CLR_WARN << "Invalid choice.\n" << CLR_RESET;
        }
    }
}

inline void intro() {
    printMenuBox("Main Menu", {"User", "Admin"}, /*showBack=*/false);
    int ch = readMenuChoice();
    if (ch==1) iUser();
    else if (ch==2) admin();
    else cout << CLR_WARN << "Goodbye!\n" << CLR_RESET;
}

inline int buildSquad(int *amount, int level, char position[10]) {
    // Choose category
    cout << "Enter the category of players you would like to purchase\n";
    cout << "1.Elite\n2.Gold\n3.Silver\n0.Back\n";
    std::string tok; cin >> tok;
    if (tok == "0" || tok == "b" || tok == "B") return -2; // back
    int ch = std::atoi(tok.c_str());

    if (ch == 1 && level < 10) {
        cout << "You cannot purchase elite players\n";
        return -1;
    }

    // Show filtered list to help the user
    auto show_list = [&](const char* cat){
        ifstream ifile("player.dat", ios::in | ios::binary);
        if (!ifile) { cout << "player.dat not found\n"; return; }
        printPlayerTableHeader();
        size_t shown = 0;
        while (true) {
            player tmp;
            if (!readPlayer(ifile, tmp)) break;
            if (iCompare(tmp.position, position) == 0 && iCompare(tmp.category, cat) == 0) {
                tmp.display(); ++shown;
            }
        }
        if (shown == 0) cout << "(no matching players)\n";
        cout << "\n";
    };

    const char* cat = (ch == 1) ? "elite" : (ch == 2) ? "gold" : (ch == 3) ? "silver" : nullptr;
    if (!cat) return -1;
    show_list(cat);

    // keep asking until: bought (return pid) OR user backs out (return -2)
    while (true) {
        int pid = buyPlayer(amount);
        if (pid == -2) return -2;   // go back to previous menu
        if (pid != -1) return pid;  // success
        // else: invalid or funds issue → re-prompt
    }
}

inline int buyPlayer(int *amount) {
    cout << "Enter the name of the player you would like to purchase (b to go back): ";
    std::string name;
    if (!(cin >> name)) {
        cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }
    if (name == "b" || name == "B") return -2; // back

    ifstream ifile("player.dat", ios::in | ios::binary);
    if (!ifile) { cout << "player.dat not found\n"; return -1; }

    bool found = false;
    player pick;
    while (true) {
        player tmp;
        if (!readPlayer(ifile, tmp)) break;
        if (iCompare(tmp.playername, name.c_str()) == 0) { found = true; pick = tmp; break; }
    }

    if (!found) {
        cout << "invalid player name. Type exactly as listed above.\n";
        return -1;
    }

    pick.display();
    if (*amount < pick.price) {
        cout << "can't purchase player — not enough funds\n";
        cout << "please purchase cheaper players to complete squad\n";
        return -1;
    }

    *amount -= pick.price;
    cout << "The player " << pick.playername << " has been successfully purchased for " << pick.price << "\n";
    cout << "remaining amount is " << *amount << "\n";
    return pick.playerid;
}

inline int team(int i, int *amount, int level) {
    int phase = (i < 3) ? 1 : (i < 6) ? 2 : (i < 10) ? 3 : 4;
    char position[10] = {0};

    switch (phase) {
        case 1: cout << "\t\t\t FORWARD\n";     strcpy(position, "fwd"); break;
        case 2: cout << "\t\t\t MIDFIELDER\n";  strcpy(position, "mid"); break;
        case 3: cout << "\t\t\t DEFENDER\n";    strcpy(position, "def"); break;
        case 4: cout << "\t\t\t GOAL KEEPER\n"; strcpy(position, "gk");  break;
        default: break;
    }
    return buildSquad(amount, level, position);
}

inline int idCheck(int x, int y) {
    ifstream ifile;
    if (x == 0) {
        user u1;
        ifile.open("user.dat", ios::in | ios::binary);
        while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
            if (y == u1.userid) return 0;
        }
        return -1;
    } else {
        ifile.open("player.dat", ios::in | ios::binary);
        while (true) {
            player tmp;
            if (!readPlayer(ifile, tmp)) break;
            if (y == tmp.playerid) return 0;
        }
        return -1;

    }
}

inline void displayLog(int e) {
    switch (e) {
        case 1: {
            ifstream ifile("user.dat", ios::in | ios::binary);
            if (!ifile) { cout << "cannot open\n"; return; }
            user u1; int count = 0;
            while (ifile.read(reinterpret_cast<char*>(&u1), sizeof(u1))) {
                u1.display(); ++count;
            }
            cout << "\n total number of users: " << count << "\n";
            break;
        }
        case 2: {
            ifstream ifile("player.dat", ios::in | ios::binary);
            if (!ifile) { cout << "cannot open\n"; return; }
            player p1; int count = 0;
            printPlayerTableHeader();
            while (ifile.read(reinterpret_cast<char*>(&p1), sizeof(p1))) {
                if (p1.playerid <= 0 || p1.playername[0] == '\0') continue;
                p1.playername[sizeof(p1.playername)-1] = '\0';
                p1.category[sizeof(p1.category)-1]     = '\0';
                p1.position[sizeof(p1.position)-1]     = '\0';
                p1.display(); ++count;
            }
            if (count == 0) cout << "(no players found)\n";
            cout << "\n total number of players: " << count << "\n";
            break;
        }
        case 3: {
            ifstream ifile("mlog.dat", ios::in | ios::binary);
            if (!ifile) { cout << "cannot open\n"; return; }
            int val; long long count = 0, sum = 0;
            while (ifile.read(reinterpret_cast<char*>(&val), sizeof(val))) {
                ++count; sum += val;
            }
            cout << "total number of matches: " << count << " (sum field=" << sum << ")\n";
            break;
        }
        case 4: {
            ifstream ifile("glog.dat", ios::in | ios::binary);
            if (!ifile) { cout << "cannot open\n"; return; }

            vector<int> per_match;
            int g;
            while (ifile.read(reinterpret_cast<char*>(&g), sizeof(g))) {
                per_match.push_back(g);
            }

            if (per_match.empty()) {
                cout << "(no goal logs)\n";
                return;
            }

            long long total = 0;
            for (size_t i = 0; i < per_match.size(); ++i) {
                cout << "match " << (i + 1) << " goals: " << per_match[i] << "\n";
                total += per_match[i];
            }
            cout << "------------------------------\n";
            cout << "TOTAL GOALS: " << total << "\n";
            cout << "AVG GOALS / MATCH: " << fixed << setprecision(2)
                 << (static_cast<double>(total) / static_cast<double>(per_match.size())) << "\n";
            break;
        }
        default:
            break;
    }
}

#endif