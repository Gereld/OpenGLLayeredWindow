#ifndef _STDAFX_H_
#define _STDAFX_H_

#include <winsdkver.h>
#ifndef _WIN32_WINNT
 	#define _WIN32_WINNT _WIN32_WINNT_MAXVER
#endif
#include <sdkddkver.h>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

#include <algorithm>
#include <array>
#include <memory>
#include <span>
#include <string_view>
#include <string>
#include <vector>

#endif // _STDAFX_H_
