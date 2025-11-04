#================================ Colors ================================

CYAN     = \033[38;5;45m
PINK     = \033[38;5;213m
RED      = \033[38;5;203m
GREEN    = \033[38;5;77m
YELLOW   = \033[38;5;220m
RESET    = \033[0m

#================================ Values ================================

NAME        = main
SRC         = main.c uart.c auth.c led.c
OBJ         = $(SRC:.c=.o)
MCU         = atmega328p # Target microcontroller
F_CPU       ?= 16000000UL # CPU clock frequeRESETy

CC          = avr-gcc       # AVR C compiler
OBJCOPY     = avr-objcopy   # Converts .bin to .hex
AVRDUDE     = avrdude       # Flashes the program to the board via serial port

CFLAGS      = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os 

PROGRAMMER  = arduino
PORT        = /dev/ttyUSB0  # Adjust depending on your computer
BAUD        = 115200

AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(NAME).hex

#================================ Rules ================================

all: hex flash

hex: $(OBJ) $(NAME).h
	@echo "$(CYAN)🔗 Linking object files...$(RESET)"
	@$(CC) $(CFLAGS) -o $(NAME).bin $(OBJ)
	@echo "$(YELLOW)🗘 Converting binary to HEX...$(RESET)"
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).bin $(NAME).hex
	@echo "$(GREEN)✔ HEX file generated: $(NAME).hex$(RESET)"

%.o: %.c
	@echo "$(CYAN)🗘 Compiling $<...$(RESET)"
	@$(CC) $(CFLAGS) -c $< -o $@

flash:
	@echo "$(PINK)→ Flashing microcontroller via avrdude...$(RESET)"
	@$(AVRDUDE) $(AVRDUDE_FLAGS)
	@echo "$(GREEN)✔ Flash completed successfully.$(RESET)"

clean:
	@echo "$(RED)🗑 Removing binary, HEX and object files...$(RESET)"
	@rm -f $(NAME).bin $(NAME).hex $(OBJ)
	@echo "$(GREEN)♻ Directory cleaned.$(RESET)"

re: clean all


.PHONY: all hex flash clean re