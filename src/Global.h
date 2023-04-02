#pragma once

//---------------------------------------------------------------------------------------
// Convenience macro for releasing COM objects.
//---------------------------------------------------------------------------------------

#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

//---------------------------------------------------------------------------------------
// Convenience macro for deleting objects.
//---------------------------------------------------------------------------------------

#define SafeDelete(x) { delete x; x = 0; }

#define ReleasePointer(x) { if(x != nullptr){ delete x; x = 0;} }
#define ReleaseArrayPointer(x) { if(x != nullptr){ delete[] x; x = 0;} }