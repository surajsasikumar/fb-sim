# CRXBYT Football Simulator
A lightweight C++17 console application that lets you manage squads, simulate football matches with a Poisson goal model, and prints lively TV-style commentary with a live score bar and pacing.
 Scoring: goals per team follow a Poisson process.
 Management: create users, buy players by role & category with a budget.
 Admin tools: insert players, view logs, calibrate league average goals.
 Nice UX: boxed menus with Back option, colorized tables, and adjustable pacing.

# Requirements
 C++17 compiler
 Linux/macOS: g++ (GCC 8+) or Clang with libc++
 Windows: MSYS2/MinGW (g++), or Visual Studio (use MSVC & adapt commands)
 Make (recommended for one-command build/run)

# Quick Start (Linux / macOS)
 
 Set secrets for Admin & VIP roles persistently:
 cat > .env.mk <<'EOF'
 ADMIN_PASS := myadmin
 VIP_PASS   := myvip
 EOF
 
 Build, seed, and run
 make run
 
 One-off run without .env.mk:
   make run ADMIN_PASS='myadmin' VIP_PASS='myvip'

# Quick Start (Windows)
Use PowerShell, cmd, or Git Bash. The Makefile detects Windows and the .exe suffix.
PowerShell / cmd (recommended):
 
 rem Optional persistent file:
 rem   echo ADMIN_PASS := myadmin > .env.mk
 rem   echo VIP_PASS   := myvip   >> .env.mk
 
 make run
 
 One-off run without .env.mk:
   make run ADMIN_PASS=myadmin VIP_PASS=myvip
   
 If you run the binary directly (e.g., double-click or .\fbsim.exe) you must set environment variables yourself; otherwise Admin/VIP features will be disabled.

# Make Targets
 make run — Seeds data, builds, runs the app (passes env secrets).
 make / make all — Seeds and builds.
 make seed — Only runs the seeder (seed_data), keeps binaries.
 make reseed — Deletes *.dat then seeds from scratch.
 make print-env — Shows what passwords make will pass in.
 make clean — Removes app, seeder, and *.dat.

# Passwords / Security
 No hard-coded passwords in source.
 App reads env vars:
 FBSIM_ADMIN_PASS — required to access Admin menu.
 FBSIM_VIP_PASS — required to enable VIP upgrades during user creation.
 Input is captured with no echo:
 macOS/Linux: termios
 Windows: GetConsoleMode / SetConsoleMode
 Compares are constant-time; 3 attempts; small backoff to throttle.
 If a variable is missing:
  Admin access is disabled: you’ll see [WARN] Admin access disabled (set FBSIM_ADMIN_PASS).
  VIP upgrades are disabled for users.

# How the Simulation Works
 Model: Football goals per team are approximated by a Poisson process.
 Expected goals (λ) per team:
 
 λ1 = attack_strength(team1) * defense_strength(team2) * league_avg_goals
 λ2 = attack_strength(team2) * defense_strength(team1) * league_avg_goals
 Attack strength = mean rating of the first 6 players (FWD+MID).
 Defense strength = mean rating of the last 5 (DEF+DEF+DEF+DEF+GK).
 League avg goals is configurable via Admin menu or from logs.
 Sampling: builds probabilities for 0..5 goals and picks among the top 3 for variety.
 Event grid: a 90-minute timeline is filled with:
 Team 1 goals (bucket 0), Team 2 goals (1)
 Team 1 attacks (2), Team 2 attacks (3)
 Team 1 defensive moments (4), Team 2 defensive moments (5)
 Commentary: varied, colorized, with a live score bar and typing effect.
 Pacing: centralized SIM_SLOW multiplier to slow down/speed up the show.

# Data Files & Formats
 player.dat — packed fixed-width records for durability:
 int playerid, prating, paggression, price
 char playername[20], category[10], position[10]
 user.dat — user id, team name, username, squad[11], etc.
 mlog.dat — append an int per match (used to count matches played).
 glog.dat — append total goals (int) per match.
 The seeder (seed_data.cpp) creates a few demo players & two users so you can immediately simulate.

# Gameplay: Menus & Flow
 Main Menu
 User → Create or Login
 Admin → Manage players, avg goals, view logs
 Use 0 or b to go Back wherever shown.
 User → Create
 Choose VIP (if enabled) or standard.
 Purchase 11 players by role (FWD/MID/DEF/GK) and category (Elite/Gold/Silver).
 Funds are deducted; lists display filtered players with tidy columns.
 User → Login → Display squad
 Prints attackers/midfielders/defenders/goalkeeper names.
 Play match (from main app menu)
 Lists available users.
 Select your userid and opponent userid (must be different).
 Watch the broadcast-style commentary with live score bar, halftime, and full-time summary. Pacing is adjustable.
 Admin
 Insert player data (append new players).
 Display player data (clean table).
 Enter league average goal (manually or compute from logs).
 Display logs:
 User list
 Player list
 Matches played (count)
 Goals per match + totals/average

# Tuning & Settings
 Pacing: In headers/FBSIM.H, adjust:
 
 inline double SIM_SLOW = 1.6; // higher = slower
 Typing speed & event sleeps are all routed through helpers (nap_ms, nap_us) so the show slows/speeds uniformly.
 Colors:
 If your terminal doesn’t support ANSI, you can set color macros to "" in the headers (search for C_/CLR_).
 Avg league goals:
 Admin → set manually, or compute from logs (mlog.dat/glog.dat).

# Troubleshooting
 [WARN] Admin access disabled (set FBSIM_ADMIN_PASS).
 You ran without env vars. Use make run ADMIN_PASS=... VIP_PASS=..., or create .env.mk, or set env in your shell.
 Admin/VIP still off when using Git Bash/MinGW on Windows
 Use make run (the Makefile injects env properly and appends .exe).
 Or set env in PowerShell/cmd and run .\fbsim.exe.
 Player names look garbled or rows repeat
 Likely a corrupted player.dat from old structure. Run: make reseed, then try again.
 Colors look weird: Use Windows Terminal or PowerShell. Or disable color macros in headers.
 “Used by another player. please enter new playerid” repeats
 You chose a duplicate id. Pick a new unique numeric id; the code checks player.dat for you.
