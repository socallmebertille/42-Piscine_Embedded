# **************************************************************************** #
#                                   Colors                                     #
# **************************************************************************** #
GREEN   = \033[0;32m
RED     = \033[0;31m
YELLOW  = \033[0;33m
CYAN    = \033[1;36m
MAGENTA = \033[0;35m
NC      = \033[0m

# **************************************************************************** #
#                                   Project                                    #
# **************************************************************************** #
NAME        = main
MCU         = atmega328p # Target microcontroller
F_CPU       ?= 16000000UL # CPU clock frequency

CC          = avr-gcc       # AVR C compiler
OBJCOPY     = avr-objcopy   # Converts .bin to .hex
AVRDUDE     = avrdude       # Flashes the program to the board via serial port

CFLAGS      = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os 

PROGRAMMER  = arduino
PORT        = /dev/ttyUSB0  # Adjust depending on your computer
BAUD        = 115200

AVRDUDE_FLAGS = -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUD) -U flash:w:$(NAME).hex

# **************************************************************************** #
#                                   Rules                                      #
# **************************************************************************** #

all: hex flash

hex: $(NAME).c
	@echo "$(CYAN)๐”ง Compiling $(NAME).c...$(NC)"
	@$(CC) $(CFLAGS) -o $(NAME).bin $(NAME).c
	@echo "$(YELLOW)๐” Converting binary to HEX...$(NC)"
	@$(OBJCOPY) -O ihex -R .eeprom $(NAME).bin $(NAME).hex
	@echo "$(GREEN)โ… HEX file generated: $(NAME).hex$(NC)"

flash:
	@echo "$(MAGENTA)โก Flashing microcontroller via avrdude...$(NC)"
	@$(AVRDUDE) $(AVRDUDE_FLAGS)
	@echo "$(GREEN)โ… Flash completed successfully.$(NC)"

clean:
	@echo "$(RED)๐งน Removing binary and HEX files...$(NC)"
	@rm -f $(NAME).bin $(NAME).hex
	@echo "$(GREEN)โ… Directory cleaned.$(NC)"

re: clean all

# **************************************************************************** #
#                                   Phony                                      #
# **************************************************************************** #
.PHONY: all hex flash clean re