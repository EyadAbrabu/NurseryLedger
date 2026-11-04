#include <cassert>
#include "Date.hpp"

int main() {
    Date validDate{2028, 2, 29};
    Date invalidDate{2026, 2, 31};

     assert(validDate.isValid());
     assert(!invalidDate.isValid());
// Leap-year boundaries
assert((Date{1900, 2, 29}.isValid() == false));
assert((Date{2000, 2, 29}.isValid() == true));

// Invalid months
assert((Date{2026, 0, 15}.isValid() == false));
assert((Date{2026, 13, 15}.isValid() == false));

// Invalid days
assert((Date{2026, 4, 31}.isValid() == false));
assert((Date{2026, 1, 0}.isValid() == false));

// Valid ordinary date
assert((Date{2026, 11, 15}.isValid() == true));
    return 0;
}
