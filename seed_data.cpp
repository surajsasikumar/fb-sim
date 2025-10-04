#include "headers/FOOTBALL.H"
#include <cstring>
#include <fstream>
#include <iostream>

static void setc(char* dst, size_t cap, const char* src) {
    size_t n = 0; while (n < cap-1 && src[n] != '\0') ++n;
    std::memcpy(dst, src, n); dst[n] = '\0';
}

int main() {
    // truncate logs
    std::ofstream("mlog.dat", std::ios::binary | std::ios::trunc).close();
    std::ofstream("glog.dat", std::ios::binary | std::ios::trunc).close();

    // truncate players file
    std::ofstream("player.dat", std::ios::binary | std::ios::trunc).close();

    // helper to add a player quickly
    auto addPlayer = [](int id, int rating, int aggr, int price,
                        const char* name, const char* cat, const char* pos) {
        player p{};
        p.playerid = id;
        p.prating = rating;
        p.paggression = aggr;
        p.price = price;
        setc(p.playername, sizeof(p.playername), name);
        setc(p.category,   sizeof(p.category),   cat);
        setc(p.position,   sizeof(p.position),   pos);
        if (!appendPlayer(p)) {
            std::cerr << "Failed to append player " << name << "\n";
        }
    };

    // Forwards
    addPlayer(101, 86, 20, 7000, "Ramos",  "elite", "fwd");
    addPlayer(102, 88, 18, 7200, "Kane",   "elite", "fwd");
    addPlayer(103, 80, 22, 5000, "Icardi", "gold",  "fwd");
    addPlayer(104, 78, 18, 4800, "Jovic",  "gold",  "fwd");

    // Midfielders
    addPlayer(201, 84, 15, 6500, "Modric", "elite", "mid");
    addPlayer(202, 79, 17, 5200, "Koke",   "gold",  "mid");
    addPlayer(203, 76, 14, 4500, "Fofana", "silver","mid");

    // Defenders
    addPlayer(301, 85, 12, 6200, "Dias",   "elite", "def");
    addPlayer(302, 81, 13, 5600, "Stones", "gold",  "def");
    addPlayer(303, 74, 16, 3800, "Sule",   "silver","def");

    // Goalkeepers
    addPlayer(401, 84, 10, 6000, "Neuer",  "elite", "gk");
    addPlayer(402, 78, 11, 4800, "Rui",    "gold",  "gk");

    // USERS
    user u1{}; u1.userid=1; u1.level=10; setc(u1.username, sizeof(u1.username), "Alice"); setc(u1.tname, sizeof(u1.tname), "Eagles"); u1.amount=30000;
    int squad1[11] = {101,102,103, 201,202,203, 301,302,303, 303,401};
    for (int i=0;i<11;i++) u1.squad[i]=squad1[i];

    user u2{}; u2.userid=2; u2.level=5; setc(u2.username, sizeof(u2.username), "Bob"); setc(u2.tname, sizeof(u2.tname), "Sharks"); u2.amount=15000;
    int squad2[11] = {103,104,101, 202,203,201, 302,303,301, 303,402};
    for (int i=0;i<11;i++) u2.squad[i]=squad2[i];

    std::ofstream uf("user.dat", std::ios::binary | std::ios::trunc);
    uf.write(reinterpret_cast<char*>(&u1), sizeof(u1));
    uf.write(reinterpret_cast<char*>(&u2), sizeof(u2));
    uf.close();

    std::cout << "Seeded player.dat and user.dat\n";
    return 0;
}