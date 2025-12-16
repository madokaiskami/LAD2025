#ifndef GREETER_H
#define GREETER_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize localization for the greeter library.
 *
 * This function binds the text domain and sets the locale so that
 * translated messages can be returned. It should be called once before
 * any other greeter calls.
 */
void greeter_init_locale(void);

/**
 * @brief Translate a message with a built-in Russian fallback.
 */
const char *greeter_translate(const char *msgid);

/**
 * @brief Produce a localized greeting message.
 *
 * @param name Name to greet. If NULL or empty, a generic greeting is returned.
 * @param buffer Output buffer that receives the greeting.
 * @param buffer_size Size of the output buffer.
 * @return 0 on success, non-zero if the buffer is too small.
 */
int greeter_build_message(const char *name, char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif // GREETER_H
