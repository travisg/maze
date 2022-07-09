#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <vector>

// From https://weblog.jamisbuck.org/2010/12/29/maze-generation-eller-s-algorithm

static size_t width = 20;
static size_t height = 20;
static long int right_open_bias = 80;
static long int down_open_bias = 50;

static bool print_set_num = false;

struct cell {
    int set;
    bool open_right;
    bool open_down;
};

struct row {
    explicit row(size_t width) {
        cells.resize(width);
    }

    std::vector<cell> cells;

    void print_top(size_t open_slot);
    void print_row();

    // routines to manipulate cells in the list

    // replace one set with another in all matching cells
    void replace_set(int new_set, int old_set);

    // count the number of cells in this set that has a downward opening
    bool set_has_downward_opening(int set);

    // pick a random cell in this set
    cell *pick_random_in_set(int set);
};

int main() {
    srandom(time(NULL));

    size_t top_open_slot;
    int entrance_set;
    int next_set = 0;
    row r(width);

    for (size_t y = 0; y < height; y++) {
        if (y == 0) {
            // first row, assign each a separate set
            for (auto &c : r.cells) {
                c.set = next_set++;
                c.open_right = false;
                c.open_down = false;
            }

            // decide which top slot is the entrance
            top_open_slot = random() % width; // XXX test that this is valid

            // remember the entrance set
            entrance_set = r.cells[top_open_slot].set;
        } else {
            // look at the row above us (previous pass) and inherit sets from open ceilings
            for (auto &c : r.cells) {
                if (!c.open_down) {
                    c.set = next_set++; // new set if the ceiling isn't open
                }

                // default to closed right wall and downward opening
                c.open_right = false;
                c.open_down = false;
            }
        }

        // randomly merge cells to the right from different sets
        for (size_t x = 0; x < width; x++) {
            auto &c = r.cells[x]; // current cell

            if (x < width - 1) {
                auto &cr = r.cells[x + 1]; // cell to the right
                if (c.set != cr.set) {
                    if ((random() % 100) < right_open_bias) {
                        // we're going to open the right wall and merge cells
                        c.open_right = true;

                        // merge with the lowest set of the two
                        auto new_set = std::min(c.set, cr.set);
                        auto old_set = std::max(c.set, cr.set);

                        // walk the set and replace all of the target ones with lower of the two
                        r.replace_set(new_set, old_set);

                        // if we replaced the entrace set, update it
                        entrance_set = (entrance_set == old_set) ? new_set : entrance_set;
                    }
                }
            }
        }

        // deal with opening downward holes
        if (y < height - 1) {
            // randomly open a downward hole
            for (auto &c : r.cells) {
                if ((random() % 100) < down_open_bias) {
                    // we're going to open downwards
                    c.open_down = true;
                }
            }

            // make another pass through and make sure every set has at least one
            // downward opening
            for (auto &c : r.cells) {
                // if this cell has an opening, then by definition this set does
                if (c.open_down) continue;

                // see if the set has an opening in any other cell and if not open it
                if (!r.set_has_downward_opening(c.set)) {
                    auto *cp = r.pick_random_in_set(c.set);
                    assert(cp);
                    cp->open_down = true;
                }
            }


        } else {
            // last row we only open once on the entrance set
            //
            // note: if the algorithm is working properly the entrance set should have made
            // its way to the bottom, which is why we look for a random cell in this set.
            auto *cp = r.pick_random_in_set(entrance_set);
            assert(cp);
            cp->open_down = true;
        }

        // output one line of maze
        if (y == 0) {
            r.print_top(top_open_slot);
        }
        r.print_row();
    }

    return 0;
}

void row::replace_set(int new_set, int old_set) {
    for (auto &c : cells) {
        if (c.set == old_set) {
            c.set = new_set;
        }
    }
}

bool row::set_has_downward_opening(int set) {
    for (auto &c : cells) {
        if (c.set == set && c.open_down) {
            return true;
        }
    }

    return false;
}

cell *row::pick_random_in_set(int set) {
    size_t count = 0;

    for (auto &c : cells) {
        if (c.set == set) {
            count++;
        }
    }

    size_t target = random() % count;
    assert(target < count);

    count = 0;
    for (auto &c : cells) {
        if (c.set == set) {
            if (count == target) {
                return &c;
            }
            count++;
        }
    }

    return nullptr;
}

void row::print_top(size_t top_open_slot) {
    for (size_t x = 0; x < width; x++) {
        printf("_");
        if (x == top_open_slot) {
            printf("    ");
        } else {
            printf("____");
        }
    }
    putchar('_');
    putchar('\n');
}

void row::print_row() {
    // inner cells
    putchar('|'); // left wall
    for (auto &c : cells) {
        printf("    ");
        putchar(c.open_right ? ' ' : '|');
    }
    putchar('\n');

    putchar('|'); // left wall
    for (auto &c : cells) {
        if (print_set_num) {
            printf(" %02x ", c.set);
        } else {
            printf("    ");
        }
        putchar(c.open_right ? ' ' : '|');
    }
    putchar('\n');

    putchar('|'); // left wall
    for (auto &c : cells) {
        printf("    ");
        putchar(c.open_right ? ' ' : '|');
    }
    putchar('\n');

    // bottom wall of cell
    putchar('-');
    for (auto &c : cells) {
        printf(c.open_down ? "    " : "----");
        putchar('-');
    }
    putchar('\n');
}
