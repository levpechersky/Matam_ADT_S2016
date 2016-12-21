CC = gcc
SRCS = customer.c realtor.c priceOffer.c yad3Service.c email.c
HEADERS = $(subst .c,.h,$(SRCS)) 
OBJS = $(subst .c,.o,$(SRCS)) 
#SRCS HEADERS and OBJS do not include files with main() 
# and headers that do not change;  
MAIN_EXEC = mtm_yad3
TESTDIR = tests
TESTS = $(TESTDIR)/customer_test.c $(TESTDIR)/priceOffer_test.c \
 $(TESTDIR)/realtor_test.c $(TESTDIR)/yad3Service_test.c $(TESTDIR)/email_test.c
EXEC = mtm_yad3 realtor_test customer_test priceOffer_test yad3Service_test \
 email_test list_test
DEBUG_FLAG = #-g
RELEASE_FLAGS = -Werror -pedantic-errors -DNDEBUG
CFLAGS = -std=c99 -Wall -pedantic -L. -lmtm -lex1 $(DEBUG_FLAG) $(RELEASE_FLAGS)

#Main app
$(MAIN_EXEC): $(OBJS) $(MAIN_EXEC).o
	$(CC) -o $@ $(OBJS) $@.o $(CFLAGS) 
#Build all tests
.PHONY: tests
tests: 
	make realtor_test
	make customer_test
	make priceOffer_test
	make yad3Service_test
	make email_test 
	make list_test

#Tests executables:
realtor_test : $(TESTDIR)/realtor_test.o realtor.o
	$(CC) -o $@ $^ $(CFLAGS) 
customer_test : $(TESTDIR)/customer_test.o customer.o
	$(CC) -o $@ $^ $(CFLAGS) 
priceOffer_test : $(TESTDIR)/priceOffer_test.o priceOffer.o email.o
	$(CC) -o $@ $^ $(CFLAGS)
yad3Service_test : $(TESTDIR)/yad3Service_test.o  $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS)
email_test : $(TESTDIR)/email_test.o email.o
	$(CC) -o $@ $^ $(CFLAGS)
list_test : $(TESTDIR)/list_test.o  list_mtm/list_mtm.o
	$(CC) -o $@ $^ $(CFLAGS)
	
#mtm_list rules: 
$(TESTDIR)/list_test.o : $(TESTDIR)/list_test.c
	$(CC) $(CFLAGS) $*.c -c -o $@ 
list_mtm/list_mtm.o : list_mtm/list_mtm.c
	$(CC) $(CFLAGS) -c $*.c -o $@
#Tests' .o files, created in tests directory:
$(TESTDIR)/%.o : $(TESTDIR)/%.c
	$(CC) $(CFLAGS) $(TESTDIR)/$*.c -c -o $@ 
#Other object files in main directory:
%.o : %.c
	$(CC) $(CFLAGS) -c $*.c 

#Misc commands	:
.PHONY: clean
# *.exe is for windows, $(EXEC) is for linux
clean:
	rm -f *.o *.exe $(TESTDIR)/*.o list_mtm/*.o  $(EXEC)
	
.PHONY: dependencies
dependencies:
	gcc -MM *.c ./tests/*.c 