/**
 * @defgroup Evas_Object_Textblock2 Textblock2 Object Functions
 *
 * Functions used to create and manipulate textblock2 objects. Unlike
 * @ref Evas_Object_Text, these handle complex text, doing multiple
 * styles and multiline text based on HTML-like tags. Of these extra
 * features will be heavier on memory and processing cost.
 *
 * @section Evas_Object_Textblock2_Tutorial Textblock2 Object Tutorial
 *
 * This part explains about the textblock2 object's API and proper usage.
 * The main user of the textblock2 object is the edje entry object in Edje, so
 * that's a good place to learn from, but I think this document is more than
 * enough, if it's not, please contact me and I'll update it.
 *
 * @subsection textblock2_intro Introduction
 * The textblock2 objects is, as implied, an object that can show big chunks of
 * text. Textblock2 supports many features including: Text formatting, automatic
 * and manual text alignment, embedding items (for example icons) and more.
 * Textblock2 has three important parts, the text paragraphs, the format nodes
 * and the cursors.
 *
 * You can use markup to format text, for example: "<font_size=50>Big!</font_size>".
 * You can also put more than one style directive in one tag:
 * "<font_size=50 color=#F00>Big and Red!</font_size>".
 * Please notice that we used "</font_size>" although the format also included
 * color, this is because the first format determines the matching closing tag's
 * name. You can also use anonymous tags, like: "<font_size=30>Big</>" which
 * just pop any type of format, but it's advised to use the named alternatives
 * instead.
 *
 * @subsection textblock2_cursors Textblock2 Object Cursors
 * A textblock2 Cursor is data type that represents
 * a position in a textblock2. Each cursor contains information about the
 * paragraph it points to, the position in that paragraph and the object itself.
 * Cursors register to textblock2 objects upon creation, this means that once
 * you created a cursor, it belongs to a specific obj and you can't for example
 * copy a cursor "into" a cursor of a different object. Registered cursors
 * also have the added benefit of updating automatically upon textblock2 changes,
 * this means that if you have a cursor pointing to a specific character, it'll
 * still point to it even after you change the whole object completely (as long
 * as the char was not deleted), this is not possible without updating, because
 * as mentioned, each cursor holds a character position. There are many
 * functions that handle cursors, just check out the evas_textblock2_cursor*
 * functions. For creation and deletion of cursors check out:
 * @see evas_object_textblock2_cursor_new()
 * @see evas_textblock2_cursor_free()
 * @note Cursors are generally the correct way to handle text in the textblock2 object, and there are enough functions to do everything you need with them (no need to get big chunks of text and processing them yourself).
 *
 * @subsection textblock2_paragraphs Textblock2 Object Paragraphs
 * The textblock2 object is made out of text splitted to paragraphs (delimited
 * by the paragraph separation character). Each paragraph has many (or none)
 * format nodes associated with it which are responsible for the formatting
 * of that paragraph.
 *
 * @subsection textblock2_format_nodes Textblock2 Object Format Nodes
 * As explained in @ref textblock2_paragraphs each one of the format nodes
 * is associated with a paragraph.
 * There are two types of format nodes, visible and invisible:
 * Visible: formats that a cursor can point to, i.e formats that
 * occupy space, for example: newlines, tabs, items and etc. Some visible items
 * are made of two parts, in this case, only the opening tag is visible.
 * A closing tag (i.e a \</tag\> tag) should NEVER be visible.
 * Invisible: formats that don't occupy space, for example: bold and underline.
 * Being able to access format nodes is very important for some uses. For
 * example, edje uses the "<a>" format to create links in the text (and pop
 * popups above them when clicked). For the textblock2 object a is just a
 * formatting instruction (how to color the text), but edje utilizes the access
 * to the format nodes to make it do more.
 * For more information, take a look at all the evas_textblock2_node_format_*
 * functions.
 * The translation of "<tag>" tags to actual format is done according to the
 * tags defined in the style, see @ref evas_textblock2_style_set
 *
 * @subsection textblock2_special_formats Special Formats
 * Textblock2 supports various format directives that can be used in markup. In
 * addition to the mentioned format directives, textblock2 allows creating
 * additional format directives using "tags" that can be set in the style see
 * @ref evas_textblock2_style_set .
 *
 * For more details see @ref evas_textblock2_style_page
 *
 * Textblock2 supports the following formats:
 * @li font - Font description in fontconfig like format, e.g: "Sans:style=Italic:lang=hi". or "Serif:style=Bold".
 * @li font_weight - Overrides the weight defined in "font". E.g: "font_weight=Bold" is the same as "font=:style=Bold". Supported weights: "normal", "thin", "ultralight", "light", "book", "medium", "semibold", "bold", "ultrabold", "black", and "extrablack".
 * @li font_style - Overrides the style defined in "font". E.g: "font_style=Italic" is the same as "font=:style=Italic". Supported styles: "normal", "oblique", and "italic".
 * @li font_width - Overrides the width defined in "font". E.g: "font_width=Condensed" is the same as "font=:style=Condensed". Supported widths: "normal", "ultracondensed", "extracondensed", "condensed", "semicondensed", "semiexpanded", "expanded", "extraexpanded", and "ultraexpanded".
 * @li lang - Overrides the language defined in "font". E.g: "lang=he" is the same as "font=:lang=he".
 * @li font_fallbacks - A comma delimited list of fonts to try if finding the main font fails.
 * @li font_size - The font size in points.
 * @li font_source - The source of the font, e.g an eet file.
 * @li color - Text color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li underline_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li underline2_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li outline_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li shadow_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li glow_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li glow2_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li strikethrough_color - color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li align - Either "auto" (meaning according to text direction), "left", "right", "center", "middle", a value between 0.0 and 1.0, or a value between 0% to 100%.
 * @li valign - Either "top", "bottom", "middle", "center", "baseline", "base", a value between 0.0 and 1.0, or a value between 0% to 100%.
 * @li wrap - "word", "char", "mixed", or "none".
 * @li left_margin - Either "reset", or a pixel value indicating the margin.
 * @li right_margin - Either "reset", or a pixel value indicating the margin.
 * @li underline - "on", "off", "single", or "double".
 * @li strikethrough - "on" or "off"
 * @li backing_color - Background color in one of the following formats: "#RRGGBB", "#RRGGBBAA", "#RGB", and "#RGBA".
 * @li backing - Enable/disable background color. ex) "on" or "off"
 * @li style - Either "off", "none", "plain", "shadow", "outline", "soft_outline", "outline_shadow", "outline_soft_shadow", "glow", "far_shadow", "soft_shadow", or "far_soft_shadow". Direction can be selected by adding "bottom_right", "bottom", "bottom_left", "left", "top_left", "top", "top_right", or "right". E.g: "style=shadow,bottom_right".
 * @li tabstops - Pixel value for tab width.
 * @li linesize - Force a line size in pixels.
 * @li linerelsize - Either a floating point value or a percentage indicating the wanted size of the line relative to the calculated size.
 * @li linegap - Force a line gap in pixels.
 * @li linerelgap - Either a floating point value or a percentage indicating the wanted size of the line relative to the calculated size.
 * @li item - Creates an empty space that should be filled by an upper layer. Use "size", "abssize", or "relsize". To define the items size, and an optional: vsize=full/ascent to define the item's position in the line.
 * @li linefill - Either a float value or percentage indicating how much to fill the line.
 * @li ellipsis - Value between 0.0-1.0 to indicate the type of ellipsis, or -1.0 to indicate ellipsis isn't wanted.
 * @li password - "on" or "off". This is used to specifically turn replacing chars with the replacement char (i.e password mode) on and off.
 *
 * @warning We don't guarantee any proper results if you create a Textblock2
 * object
 * without setting the evas engine.
 *
 * @todo put here some usage examples
 *
 * @ingroup Evas_Object_Specific
 *
 * @{
 */

/**
 * @typedef Evas_Textblock2_Cursor
 *
 * A textblock2 cursor object, used to maipulate the cursor of an evas textblock2
 * @see evas_object_textblock2_cursor_new
 *
 */
typedef struct _Evas_Textblock2_Cursor             Evas_Textblock2_Cursor;

typedef struct _Evas_Textblock2_Rectangle          Evas_Textblock2_Rectangle;
struct _Evas_Textblock2_Rectangle
{
   Evas_Coord x, y, w, h;
};

/**
 * Text type for evas textblock2.
 */
typedef enum _Evas_Textblock2_Text_Type
{
   EVAS_TEXTBLOCK2_TEXT_RAW, /**< textblock2 text of type raw */
   EVAS_TEXTBLOCK2_TEXT_PLAIN, /**< textblock2 text of type plain */
   EVAS_TEXTBLOCK2_TEXT_MARKUP /**< textblock2 text of type markup */
} Evas_Textblock2_Text_Type;

/**
 * Cursor type for evas textblock2.
 */
typedef enum _Evas_Textblock2_Cursor_Type
{
   EVAS_TEXTBLOCK2_CURSOR_UNDER, /**< cursor type is under */
   EVAS_TEXTBLOCK2_CURSOR_BEFORE /**< cursor type is before */
} Evas_Textblock2_Cursor_Type;

/**
 * Returns the unescaped version of escape.
 * @param escape the string to be escaped
 * @return the unescaped version of escape
 */
EAPI const char                              *evas_textblock2_escape_string_get(const char *escape) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * Returns the escaped version of the string.
 * @param string to escape
 * @param len_ret the len of the part of the string that was used.
 * @return the escaped string.
 */
EAPI const char                              *evas_textblock2_string_escape_get(const char *string, int *len_ret) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * Return the unescaped version of the string between start and end.
 *
 * @param escape_start the start of the string.
 * @param escape_end the end of the string.
 * @return the unescaped version of the range
 */
EAPI const char                              *evas_textblock2_escape_string_range_get(const char *escape_start, const char *escape_end) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Prepends markup to the cursor cur.
 *
 * @note assumes text does not include the unicode object replacement char (0xFFFC)
 *
 * @param cur  the cursor to prepend to.
 * @param text the markup text to prepend.
 * @return Return no value.
 */
EAPI void                                     evas_object_textblock2_text_markup_prepend(Evas_Textblock2_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);

/**
 * Free the cursor and unassociate it from the object.
 * @note do not use it to free unassociated cursors.
 *
 * @param cur the cursor to free.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_free(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Sets the cursor to the start of the first text node.
 *
 * @param cur the cursor to update.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_paragraph_first(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * sets the cursor to the end of the last text node.
 *
 * @param cur the cursor to set.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_paragraph_last(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Advances to the start of the next text node
 *
 * @param cur the cursor to update
 * @return @c EINA_TRUE if it managed to advance a paragraph, @c EINA_FALSE
 * otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_paragraph_next(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Advances to the end of the previous text node
 *
 * @param cur the cursor to update
 * @return @c EINA_TRUE if it managed to advance a paragraph, @c EINA_FALSE
 * otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_paragraph_prev(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Advances 1 char forward.
 *
 * @param cur the cursor to advance.
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_char_next(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Advances 1 char backward.
 *
 * @param cur the cursor to advance.
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_char_prev(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Moves the cursor to the start of the word under the cursor.
 *
 * @param cur the cursor to move.
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise.
 * @since 1.2
 */
EAPI Eina_Bool                                evas_textblock2_cursor_word_start(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Moves the cursor to the end of the word under the cursor.
 *
 * @param cur the cursor to move.
 * @return @c EINA_TRUE on success @c EINA_FALSE otherwise.
 * @since 1.2
 */
EAPI Eina_Bool                                evas_textblock2_cursor_word_end(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Go to the first char in the node the cursor is pointing on.
 *
 * @param cur the cursor to update.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_paragraph_char_first(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Go to the last char in a text node.
 *
 * @param cur the cursor to update.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_paragraph_char_last(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Go to the start of the current line
 *
 * @param cur the cursor to update.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_line_char_first(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Go to the end of the current line.
 *
 * @param cur the cursor to update.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_line_char_last(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Return the current cursor pos.
 *
 * @param cur the cursor to take the position from.
 * @return the position or -1 on error
 */
EAPI int                                      evas_textblock2_cursor_pos_get(const Evas_Textblock2_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * Set the cursor pos.
 *
 * @param cur the cursor to be set.
 * @param pos the pos to set.
 */
EAPI void                                     evas_textblock2_cursor_pos_set(Evas_Textblock2_Cursor *cur, int pos) EINA_ARG_NONNULL(1);

/**
 * Go to the start of the line passed
 *
 * @param cur cursor to update.
 * @param line numer to set.
 * @return @c EINA_TRUE on success, @c EINA_FALSE on error.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_line_set(Evas_Textblock2_Cursor *cur, int line) EINA_ARG_NONNULL(1);

/**
 * Compare two cursors.
 *
 * @param cur1 the first cursor.
 * @param cur2 the second cursor.
 * @return -1 if cur1 < cur2, 0 if cur1 == cur2 and 1 otherwise.
 */
EAPI int                                      evas_textblock2_cursor_compare(const Evas_Textblock2_Cursor *cur1, const Evas_Textblock2_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Make cur_dest point to the same place as cur. Does not work if they don't
 * point to the same object.
 *
 * @param cur the source cursor.
 * @param cur_dest destination cursor.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_copy(const Evas_Textblock2_Cursor *cur, Evas_Textblock2_Cursor *cur_dest) EINA_ARG_NONNULL(1, 2);

/**
 * Adds text to the current cursor position and set the cursor to *before*
 * the start of the text just added.
 *
 * @param cur the cursor to where to add text at.
 * @param text the text to add.
 * @return Returns the len of the text added.
 * @see evas_textblock2_cursor_text_prepend()
 */
EAPI int                                      evas_textblock2_cursor_text_append(Evas_Textblock2_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);

/**
 * Adds text to the current cursor position and set the cursor to *after*
 * the start of the text just added.
 *
 * @param cur the cursor to where to add text at.
 * @param text the text to add.
 * @return Returns the len of the text added.
 * @see evas_textblock2_cursor_text_append()
 */
EAPI int                                      evas_textblock2_cursor_text_prepend(Evas_Textblock2_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);

/**
 * Adds format to the current cursor position. If the format being added is a
 * visible format, add it *before* the cursor position, otherwise, add it after.
 * This behavior is because visible formats are like characters and invisible
 * should be stacked in a way that the last one is added last.
 *
 * This function works with native formats, that means that style defined
 * tags like <br> won't work here. For those kind of things use markup prepend.
 *
 * @param cur the cursor to where to add format at.
 * @param format the format to add.
 * @return Returns true if a visible format was added, false otherwise.
 * @see evas_textblock2_cursor_format_prepend()
 */

/**
 * Check if the current cursor position points to the terminating null of the
 * last paragraph. (shouldn't be allowed to point to the terminating null of
 * any previous paragraph anyway.
 *
 * @param cur the cursor to look at.
 * @return @c EINA_TRUE if the cursor points to the terminating null, @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_format_append(Evas_Textblock2_Cursor *cur, const char *format) EINA_ARG_NONNULL(1, 2);

/**
 * Adds format to the current cursor position. If the format being added is a
 * visible format, add it *before* the cursor position, otherwise, add it after.
 * This behavior is because visible formats are like characters and invisible
 * should be stacked in a way that the last one is added last.
 * If the format is visible the cursor is advanced after it.
 *
 * This function works with native formats, that means that style defined
 * tags like <br> won't work here. For those kind of things use markup prepend.
 *
 * @param cur the cursor to where to add format at.
 * @param format the format to add.
 * @return Returns true if a visible format was added, false otherwise.
 * @see evas_textblock2_cursor_format_prepend()
 */
EAPI Eina_Bool                                evas_textblock2_cursor_format_prepend(Evas_Textblock2_Cursor *cur, const char *format) EINA_ARG_NONNULL(1, 2);

/**
 * Delete the character at the location of the cursor. If there's a format
 * pointing to this position, delete it as well.
 *
 * @param cur the cursor pointing to the current location.
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_char_delete(Evas_Textblock2_Cursor *cur) EINA_ARG_NONNULL(1);

/**
 * Delete the range between cur1 and cur2.
 *
 * @param cur1 one side of the range.
 * @param cur2 the second side of the range
 * @return Returns no value.
 */
EAPI void                                     evas_textblock2_cursor_range_delete(Evas_Textblock2_Cursor *cur1, Evas_Textblock2_Cursor *cur2) EINA_ARG_NONNULL(1, 2);

/**
 * Return the text of the paragraph cur points to - returns the text in markup.
 *
 * @param cur the cursor pointing to the paragraph.
 * @return the text on success, @c NULL otherwise.
 */
EAPI const char                              *evas_textblock2_cursor_paragraph_text_get(const Evas_Textblock2_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * Return the length of the paragraph, cheaper the eina_unicode_strlen()
 *
 * @param cur the position of the paragraph.
 * @return the length of the paragraph on success, -1 otehrwise.
 */
EAPI int                                      evas_textblock2_cursor_paragraph_text_length_get(const Evas_Textblock2_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * Return the currently visible range.
 *
 * @param start the start of the range.
 * @param end the end of the range.
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 * @since 1.1
 */
EAPI Eina_Bool                                evas_textblock2_cursor_visible_range_get(Evas_Textblock2_Cursor *start, Evas_Textblock2_Cursor *end) EINA_ARG_NONNULL(1, 2);

/**
 * Return the format nodes in the range between cur1 and cur2.
 *
 * @param cur1 one side of the range.
 * @param cur2 the other side of the range
 * @return the foramt nodes in the range. You have to free it.
 * @since 1.1
 */
EAPI Eina_List                               *evas_textblock2_cursor_range_formats_get(const Evas_Textblock2_Cursor *cur1, const Evas_Textblock2_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Return the text in the range between cur1 and cur2
 *
 * @param cur1 one side of the range.
 * @param cur2 the other side of the range
 * @return the text in the range
 * @see elm_entry_markup_to_utf8()
 */
EAPI char                                    *evas_textblock2_cursor_range_text_get(const Evas_Textblock2_Cursor *cur1, const Evas_Textblock2_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Return the content of the cursor.
 *
 * Free the returned string pointer when done (if it is not NULL).
 *
 * @param cur the cursor
 * @return the text in the range, terminated by a nul byte (may be utf8).
 */
EAPI char                                    *evas_textblock2_cursor_content_get(const Evas_Textblock2_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_MALLOC;

/**
 * Returns the geometry of two cursors ("split cursor"), if logical cursor is
 * between LTR/RTL text, also considering paragraph direction.
 * Upper cursor is shown for the text of the same direction as paragraph,
 * lower cursor - for opposite.
 *
 * Split cursor geometry is valid only  in '|' cursor mode.
 * In this case @c EINA_TRUE is returned and cx2, cy2, cw2, ch2 are set,
 * otherwise it behaves like cursor_geometry_get.
 *
 * @param[in] cur the cursor.
 * @param[out] cx the x of the cursor (or upper cursor)
 * @param[out] cy the y of the cursor (or upper cursor)
 * @param[out] cw the width of the cursor (or upper cursor)
 * @param[out] ch the height of the cursor (or upper cursor)
 * @param[out] cx2 the x of the lower cursor
 * @param[out] cy2 the y of the lower cursor
 * @param[out] cw2 the width of the lower cursor
 * @param[out] ch2 the height of the lower cursor
 * @param[in] ctype the type of the cursor.
 * @return @c EINA_TRUE for split cursor, @c EINA_FALSE otherwise
 * @since 1.8
 */
EAPI Eina_Bool
evas_textblock2_cursor_geometry_bidi_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch, Evas_Coord *cx2, Evas_Coord *cy2, Evas_Coord *cw2, Evas_Coord *ch2, Evas_Textblock2_Cursor_Type ctype);

/**
 * Returns the geometry of the cursor. Depends on the type of cursor requested.
 * This should be used instead of char_geometry_get because there are weird
 * special cases with BiDi text.
 * in '_' cursor mode (i.e a line below the char) it's the same as char_geometry
 * get, except for the case of the last char of a line which depends on the
 * paragraph direction.
 *
 * in '|' cursor mode (i.e a line between two chars) it is very variable.
 * For example consider the following visual string:
 * "abcCBA" (ABC are rtl chars), a cursor pointing on A should actually draw
 * a '|' between the c and the C.
 *
 * @param cur the cursor.
 * @param cx the x of the cursor
 * @param cy the y of the cursor
 * @param cw the width of the cursor
 * @param ch the height of the cursor
 * @param dir the direction of the cursor, can be NULL.
 * @param ctype the type of the cursor.
 * @return line number of the char on success, -1 on error.
 */
EAPI int                                      evas_textblock2_cursor_geometry_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch, Evas_BiDi_Direction *dir, Evas_Textblock2_Cursor_Type ctype) EINA_ARG_NONNULL(1);

/**
 * Returns the geometry of the char at cur.
 *
 * @param cur the position of the char.
 * @param cx the x of the char.
 * @param cy the y of the char.
 * @param cw the w of the char.
 * @param ch the h of the char.
 * @return line number of the char on success, -1 on error.
 */
EAPI int                                      evas_textblock2_cursor_char_geometry_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);

/**
 * Returns the geometry of the pen at cur.
 *
 * @param cur the position of the char.
 * @param cpen_x the pen_x of the char.
 * @param cy the y of the char.
 * @param cadv the adv of the char.
 * @param ch the h of the char.
 * @return line number of the char on success, -1 on error.
 */
EAPI int                                      evas_textblock2_cursor_pen_geometry_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cpen_x, Evas_Coord *cy, Evas_Coord *cadv, Evas_Coord *ch) EINA_ARG_NONNULL(1);

/**
 * Returns the geometry of the line at cur.
 *
 * @param cur the position of the line.
 * @param cx the x of the line.
 * @param cy the y of the line.
 * @param cw the width of the line.
 * @param ch the height of the line.
 * @return line number of the line on success, -1 on error.
 */
EAPI int                                      evas_textblock2_cursor_line_geometry_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);

/**
 * Set the position of the cursor according to the X and Y coordinates.
 *
 * @param cur the cursor to set.
 * @param x coord to set by.
 * @param y coord to set by.
 * @return @c EINA_TRUE on success, @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_char_coord_set(Evas_Textblock2_Cursor *cur, Evas_Coord x, Evas_Coord y) EINA_ARG_NONNULL(1);

/**
 * Set the cursor position according to the y coord.
 *
 * @param cur the cur to be set.
 * @param y the coord to set by.
 * @return the line number found, -1 on error.
 */
EAPI int                                      evas_textblock2_cursor_line_coord_set(Evas_Textblock2_Cursor *cur, Evas_Coord y) EINA_ARG_NONNULL(1);

/**
 * Get the geometry of a range.
 *
 * @param cur1 one side of the range.
 * @param cur2 other side of the range.
 * @return a list of Rectangles representing the geometry of the range.
 */
EAPI Eina_List                               *evas_textblock2_cursor_range_geometry_get(const Evas_Textblock2_Cursor *cur1, const Evas_Textblock2_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Get the simple geometry of a range.
 * The simple geometry is the geomtry in which rectangles in middle
 * lines of range are merged into one big rectangle.
 *
 * @param cur1 one side of the range.
 * @param cur2 other side of the range.
 * @return an iterator of rectangles representing the geometry of the range.
 */
EAPI Eina_Iterator                               *evas_textblock2_cursor_range_simple_geometry_get(const Evas_Textblock2_Cursor *cur1, const Evas_Textblock2_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2);

/**
 * Get the geometry of ?
 *
 * @param cur one side of the range.
 * @param cur2 other side of the range.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_format_item_geometry_get(const Evas_Textblock2_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);

/**
 * Checks if the cursor points to the end of the line.
 *
 * @param cur the cursor to check.
 * @return @c EINA_TRUE if true, @c EINA_FALSE otherwise.
 */
EAPI Eina_Bool                                evas_textblock2_cursor_eol_get(const Evas_Textblock2_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1);

/**
 * @}
 */

#include "canvas/evas_textblock2.eo.h"

/**
 * @ingroup Evas_Object_Textblock2
 *
 * @{
 */

/**
 * Adds a textblock2 to the given evas.
 * @param   e The given evas.
 * @return  The new textblock2 object.
 */
EAPI Evas_Object                             *evas_object_textblock2_add(Evas *e) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_MALLOC;

/**
 * Return the plain version of the markup.
 *
 * Works as if you set the markup to a textblock2 and then retrieve the plain
 * version of the text. i.e: <br> and <\n> will be replaced with \n, &...; with
 * the actual char and etc.
 *
 * @param obj The textblock2 object to work with. (if @c NULL, tries the
 * default).
 * @param text The markup text (if @c NULL, return @c NULL).
 * @return An allocated plain text version of the markup.
 * @since 1.2
 */
EAPI char                                    *evas_textblock2_text_markup_to_utf8(const Evas_Object *obj, const char *text) EINA_WARN_UNUSED_RESULT EINA_MALLOC;

/**
 * Return the markup version of the plain text.
 *
 * Replaces \\n -\> \<br/\> \\t -\> \<tab/\> and etc. Generally needed before you pass
 * plain text to be set in a textblock2.
 *
 * @param obj the textblock2 object to work with (if @c NULL, it just does the
 * default behaviour, i.e with no extra object information).
 * @param text The plain text (if @c NULL, return @c NULL).
 * @return An allocated markup version of the plain text.
 * @since 1.2
 */
EAPI char                                    *evas_textblock2_text_utf8_to_markup(const Evas_Object *obj, const char *text) EINA_WARN_UNUSED_RESULT EINA_MALLOC;

/**
 * Clear the textblock2 object.
 * @note Does *NOT* free the Evas object itself.
 *
 * @param obj the object to clear.
 * @return nothing.
 */
EAPI void                                     evas_object_textblock2_clear(Evas_Object *obj) EINA_ARG_NONNULL(1);

#include "canvas/evas_textblock2.eo.legacy.h"

/**
 * @}
 */
