// Copyright (C) 2024-2025 Leah Neukirchen <leah@vuxu.org>
// SPDX-License-Identifier: Apache-2.0

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "scrape.h"
#include "util.h"

// size of input buffer for paths and lines
#define BUF_SIZE 256

static void pressure_collect(scrape_req *req, void *ctx);

const struct collector pressure_collector = {
  .name = "pressure",
  .collect = pressure_collect,
};

static void pressure_collect(scrape_req *req, void *ctx) {
  (void) ctx;

  FILE *f;
  long long ms;

  char line[BUF_SIZE];

  if ((f = fopen("/proc/pressure/cpu", "r"))) {
    while (fgets_line(line, sizeof line, f)) {
      if (sscanf(line, "some %*[^t]total=%lld\n", &ms) == 1)
        scrape_write(req, "node_pressure_cpu_waiting_seconds_total", 0, ms / 1e6);
    }
    fclose(f);
  }

  if ((f = fopen("/proc/pressure/memory", "r"))) {
    while (fgets_line(line, sizeof line, f)) {
      if (sscanf(line, "some %*[^t]total=%lld\n", &ms) == 1)
        scrape_write(req, "node_pressure_memory_waiting_seconds_total", 0, ms / 1e6);
      else if (sscanf(line, "full %*[^t]total=%lld\n", &ms) == 1)
        scrape_write(req, "node_pressure_memory_stalled_seconds_total", 0, ms / 1e6);
    }
    fclose(f);
  }

  if ((f = fopen("/proc/pressure/io", "r"))) {
    while (fgets_line(line, sizeof line, f)) {
      if (sscanf(line, "some %*[^t]total=%lld\n", &ms) == 1)
        scrape_write(req, "node_pressure_io_waiting_seconds_total", 0, ms / 1e6);
      else if (sscanf(line, "full %*[^t]total=%lld\n", &ms) == 1)
        scrape_write(req, "node_pressure_io_stalled_seconds_total", 0, ms / 1e6);
    }
    fclose(f);
  }
}
