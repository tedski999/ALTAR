#include <ALTAR/ALTAR.h>

void altar_main() {
	altar_log(ALTAR_NOTEWORTHY_LOG, "log");
	altar_assert(1, "assertion");
	//altar_error("error");
}

