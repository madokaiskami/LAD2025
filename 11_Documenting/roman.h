\
    /** @file roman.h
     *  @brief Roman numeral conversion API (1..100).
     */
    #ifndef ROMAN_H
    #define ROMAN_H

    #ifdef __cplusplus
    extern "C" {
    #endif

    /**
     * @brief Convert Arabic integer to Roman numeral string (1..100).
     *
     * @param value Integer value in range [1, 100].
     * @return Pointer to static string with Roman numeral or NULL on error.
     *
     * The returned pointer is valid until program termination and must not be freed.
     */
    const char *arabic_to_roman(int value);

    /**
     * @brief Convert Roman numeral string to Arabic integer.
     *
     * @param s Null-terminated string with Roman numeral.
     * @return Integer in range [1, 100] on success, or -1 on error.
     */
    int roman_to_arabic(const char *s);

    #ifdef __cplusplus
    }
    #endif

    #endif /* ROMAN_H */
