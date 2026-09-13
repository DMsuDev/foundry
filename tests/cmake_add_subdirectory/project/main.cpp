// -----------------------------------------------------------------------------
//  Copyright (c) 2026 @DMsuDev. Licensed under the MIT License.
//  See LICENSE file in the project root for full license text.
// -----------------------------------------------------------------------------

#include <foundry/memory/scope_guard.h>
#include <iostream>

int main()
{
    { // <-- Scope guard example
        FOUNDRY_SCOPE_EXIT {
            std::cout << "Scope exited" << std::endl;
        };
    } // <-- End of scope guard example
    return 0;
}
