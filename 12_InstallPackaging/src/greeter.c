#include "greeter.h"

#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEXT_DOMAIN "greeter"

#ifndef INSTALL_LOCALE_DIR
#define INSTALL_LOCALE_DIR "../locale"
#endif

#ifndef BUILD_LOCALE_DIR
#define BUILD_LOCALE_DIR "../locale"
#endif

static int language_is_ru(void) {
    const char *language = getenv("LANGUAGE");
    if (language == NULL || language[0] == '\0') {
        language = getenv("LANG");
    }
    return language != NULL && strncmp(language, "ru", 2) == 0;
}

static const char *fallback_ru(const char *msgid) {
    if (strcmp(msgid, "Hello, traveler!") == 0) {
        return "Здравствуйте, путешественник!";
    }
    if (strcmp(msgid, "Hello, %s!") == 0) {
        return "Здравствуйте, %s!";
    }
    if (strcmp(msgid, "Usage: %s [--name NAME]\n") == 0) {
        return "Использование: %s [--name ИМЯ]\n";
    }
    if (strcmp(msgid, "Options:\n") == 0) {
        return "Параметры:\n";
    }
    if (strcmp(msgid, "  --name, -n   Name to greet\n") == 0) {
        return "  --name, -n   Имя для приветствия\n";
    }
    if (strcmp(msgid, "  --help, -h   Show this help message\n") == 0) {
        return "  --help, -h   Показать эту подсказку\n";
    }
    if (strcmp(msgid, "Name too long.\n") == 0) {
        return "Слишком длинное имя.\n";
    }
    return msgid;
}

static int use_ru_fallback = 0;

const char *greeter_translate(const char *msgid) {
    const char *translated = gettext(msgid);
    if (translated != NULL && strcmp(translated, msgid) != 0) {
        return translated;
    }
    if (use_ru_fallback && language_is_ru()) {
        return fallback_ru(msgid);
    }
    return msgid;
}

void greeter_init_locale(void) {
    const char *debug = getenv("GREETER_DEBUG");
    const char *locale = setlocale(LC_ALL, "");
    if (locale == NULL) {
        locale = setlocale(LC_ALL, "en_US.UTF-8");
    }
    if (locale == NULL) {
        locale = setlocale(LC_ALL, "C.UTF-8");
    }
    if (locale == NULL) {
        locale = setlocale(LC_ALL, "C.utf8");
    }
    if (locale == NULL) {
        setlocale(LC_ALL, "C");
    }
    if (debug != NULL) {
        fprintf(stderr, "greeter locale: %s\n", locale != NULL ? locale : "(null)");
    }
    const char *override = getenv("GREETER_LOCALE_DIR");
    const char *candidates[] = {BUILD_LOCALE_DIR, INSTALL_LOCALE_DIR};
    const char *chosen = INSTALL_LOCALE_DIR;

    if (override != NULL && override[0] != '\0' && access(override, R_OK) == 0) {
        chosen = override;
    } else {
        for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
            if (candidates[i][0] != '\0' && access(candidates[i], R_OK) == 0) {
                chosen = candidates[i];
                break;
            }
        }
    }

    bindtextdomain(TEXT_DOMAIN, chosen);
    textdomain(TEXT_DOMAIN);
    if (language_is_ru()) {
        const char *probe = gettext("Hello, traveler!");
        use_ru_fallback = (probe == NULL || strcmp(probe, "Hello, traveler!") == 0);
    }
    if (debug != NULL) {
        fprintf(stderr, "greeter locale dir: %s\n", chosen);
    }
}

int greeter_build_message(const char *name, char *buffer, size_t buffer_size) {
    const char *template = NULL;
    if (name == NULL || name[0] == '\0') {
        template = greeter_translate("Hello, traveler!");
        size_t needed = strlen(template) + 1;
        if (needed > buffer_size) {
            return 1;
        }
        snprintf(buffer, buffer_size, "%s", template);
    } else {
        template = greeter_translate("Hello, %s!");
        size_t needed = strlen(template) - 2 + strlen(name) + 1; // %s replaced by name
        if (needed > buffer_size) {
            return 1;
        }
        snprintf(buffer, buffer_size, template, name);
    }
    return 0;
}
