COMPILER = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I headers

TARGET   = fbsim
SEED_BIN = seed_data

# --- Secrets (override on CLI or via .env.mk) ---
ADMIN_PASS ?=
VIP_PASS   ?=
-include .env.mk

# Detect Windows without calling uname
WINDOWS := 0
ifeq ($(OS),Windows_NT)
  WINDOWS := 1
endif
ifdef ComSpec
  WINDOWS := 1
endif

ifeq ($(WINDOWS),1)
  EXE := .exe
  RUN_CMD  = cmd /C "set FBSIM_ADMIN_PASS=$(ADMIN_PASS) && set FBSIM_VIP_PASS=$(VIP_PASS) && .\\$(TARGET)$(EXE)"
  SEED_CMD = cmd /C ".\\$(SEED_BIN)$(EXE)"
  RM = del /F /Q
else
  EXE :=
  RUN_CMD  = FBSIM_ADMIN_PASS="$(ADMIN_PASS)" FBSIM_VIP_PASS="$(VIP_PASS)" ./$(TARGET)$(EXE)
  SEED_CMD = ./$(SEED_BIN)$(EXE)
  RM = rm -f
endif

.PHONY: all run clean data seed reseed print-env

all: data $(TARGET)$(EXE)

$(TARGET)$(EXE): project.cpp headers/FBSIM.H headers/FOOTBALL.H headers/POISSON.H
	$(COMPILER) $(CXXFLAGS) $(INCLUDES) project.cpp -o $(TARGET)$(EXE)

$(SEED_BIN)$(EXE): seed_data.cpp headers/FOOTBALL.H
	$(COMPILER) $(CXXFLAGS) $(INCLUDES) seed_data.cpp -o $(SEED_BIN)$(EXE)

data: $(SEED_BIN)$(EXE)
	@echo "Seeding data files..."
	@$(SEED_CMD)

seed: $(SEED_BIN)$(EXE)
	@$(SEED_CMD)

reseed: $(SEED_BIN)$(EXE)
	@$(RM) *.dat
	@$(SEED_CMD)

run: all
	@$(RUN_CMD)

print-env:
	@echo "FBSIM_ADMIN_PASS=$(ADMIN_PASS)"
	@echo "FBSIM_VIP_PASS=$(VIP_PASS)"

clean:
	@$(RM) $(TARGET)$(EXE) $(SEED_BIN)$(EXE) *.o *.dat
