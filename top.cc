/*#include "Tile-debug-log.h"*/
#include "Tile.h"
#include "mm.h"
#include <cassert>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <uint.h>
#include <unistd.h>

using namespace std;

static uint64_t trace_count = 0;
static uint64_t main_time = 0;
Tile *top;
mm_magic_t *mem;

void tick(bool verbose, bool done_reset, std::ofstream &debugLogFile) {
  main_time++;

  top->io_nasti_aw_ready = UInt<1>(mem->aw_ready());
  top->io_nasti_ar_ready = UInt<1>(mem->ar_ready());
  top->io_nasti_w_ready = UInt<1>(mem->w_ready());
  top->io_nasti_b_valid = UInt<1>(mem->b_valid());
  top->io_nasti_b_bits_id = UInt<5>(mem->b_id());
  top->io_nasti_b_bits_resp = UInt<2>(mem->b_resp());
  top->io_nasti_r_valid = UInt<1>(mem->r_valid());
  top->io_nasti_r_bits_id = UInt<5>(mem->r_id());
  top->io_nasti_r_bits_resp = UInt<2>(mem->r_resp());
  top->io_nasti_r_bits_last = UInt<1>(mem->r_last());
  memcpy(&top->io_nasti_r_bits_data, mem->r_data(), 8);

  top->eval(true, verbose, done_reset, debugLogFile, main_time);
  mem->tick(top->reset, top->io_nasti_ar_valid,
            top->io_nasti_ar_bits_addr.as_single_word(),
            top->io_nasti_ar_bits_id.as_single_word(),
            top->io_nasti_ar_bits_size.as_single_word(),
            top->io_nasti_ar_bits_len.as_single_word(), top->io_nasti_aw_valid,
            top->io_nasti_aw_bits_addr.as_single_word(),
            top->io_nasti_aw_bits_id.as_single_word(),
            top->io_nasti_aw_bits_size.as_single_word(),
            top->io_nasti_aw_bits_len.as_single_word(), top->io_nasti_w_valid,
            top->io_nasti_w_bits_strb.as_single_word(),
            &top->io_nasti_w_bits_data, top->io_nasti_w_bits_last,
            top->io_nasti_r_ready, top->io_nasti_b_ready);
  main_time++;
}

int main(int argc, char **argv) {
  std::ofstream debugLogFile("simulation_log.txt");

  uint64_t timeout = 100000L;
  top = new Tile;
  mem = new mm_magic_t(1L << 32, 8);
  cout << "Enabling waves..." << endl;
  load_mem(mem->get_data(), (const char *)(argv[1]));
  top->reset = UInt<1>(1);
  cout << "Starting simulation!" << endl;
  for (size_t i = 0; i < 5; i++) {
    tick(true, false, debugLogFile);
  }

  top->reset = UInt<1>(0);
  top->io_host_fromhost_bits = UInt<32>(0);
  top->io_host_fromhost_valid = UInt<1>(0);
  do {
    tick(true, true, debugLogFile);
    /*bool flags = top->PARTflags[1];*/
    /*assert(top->core.dpath.illegal.as_single_word() == 0 && "Illegal
     * instruction detected!");*/
  } while (!top->io_host_tohost.as_single_word() && main_time < timeout);
  cout << "io_host_tohost " << top->io_host_tohost.as_single_word() << endl;
  cout << "main_time " << main_time << endl;
  cout << "done" << endl;

  for (size_t i = 0; i < 10; i++) {
    tick(true, true, debugLogFile);
  }

  debugLogFile.close();
  delete top;
  delete mem;
  return 0;
}
