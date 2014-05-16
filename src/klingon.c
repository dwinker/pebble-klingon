// $Revision: 1.2 $
#include "pebble.h"

Window    *window;
BitmapLayer *background_layer;
GBitmap *background_bitmap;
TextLayer *text_hours_layer;
TextLayer *text_minutes_layer;
TextLayer *text_seconds_layer;

// This works around the inability to use the current GRect macro for constants.
#define ConstantGRect(x, y, w, h) {{(x), (y)}, {(w), (h)}}
GRect HOURS_RECT   = ConstantGRect(0,   5, 144, 70);
GRect MINUTES_RECT = ConstantGRect(0,  68, 144, 50);
GRect SECONDS_RECT = ConstantGRect(0, 125, 144, 30);

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {

    // Need to be static because it's used by the system later.
    // "4 6" is particularily wide in Klingon font.
    static char str_hour[] = "4 6";
    static char str_min[]  = "4 6";
    static char str_sec[]  = "4 6";
    static int  last_min  = -1;
    static int  last_hour = -1;

    str_sec[0] = '0' + tick_time->tm_sec / 10;
    str_sec[2] = '0' + tick_time->tm_sec % 10;
    text_layer_set_text(text_seconds_layer, str_sec);

    if(last_min != tick_time->tm_min) {
        last_min = tick_time->tm_min;
        str_min[0] = '0' + tick_time->tm_min / 10;
        str_min[2] = '0' + tick_time->tm_min % 10;
        text_layer_set_text(text_minutes_layer, str_min);
    }

    if(last_hour != tick_time->tm_hour) {
        last_hour = tick_time->tm_hour;
        str_hour[0] = '0' + tick_time->tm_hour / 10;
        str_hour[2] = '0' + tick_time->tm_hour % 10;
        text_layer_set_text(text_hours_layer, str_hour);
    }

    //app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, "handle_second_tick (h m s): \"%s\" \"%s\" \"%s\"", str_hour, str_min, str_sec);
}

static void init(void) {

    Layer *window_layer;
    GRect bounds;

    window = window_create();
    window_stack_push(window, true /* Animated */);

    window_layer = window_get_root_layer(window);
    bounds = layer_get_frame(window_layer);
    background_layer = bitmap_layer_create(bounds);
    background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    bitmap_layer_set_bitmap(background_layer, background_bitmap);
    layer_add_child(window_layer, bitmap_layer_get_layer(background_layer));

    // Would have liked to have used 60 as hours font size, but 53 is the biggest that works right.
    text_hours_layer = text_layer_create(HOURS_RECT);
    text_layer_set_text_color(text_hours_layer, GColorWhite);
    text_layer_set_background_color(text_hours_layer, GColorClear);
    text_layer_set_text_alignment(text_hours_layer, GTextAlignmentCenter);
    text_layer_set_font(text_hours_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KLINGON_53)));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_hours_layer));

    text_minutes_layer = text_layer_create(MINUTES_RECT);
    text_layer_set_text_color(text_minutes_layer, GColorWhite);
    text_layer_set_background_color(text_minutes_layer, GColorClear);
    text_layer_set_text_alignment(text_minutes_layer, GTextAlignmentCenter);
    text_layer_set_font(text_minutes_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KLINGON_40)));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_minutes_layer));

    text_seconds_layer = text_layer_create(SECONDS_RECT);
    text_layer_set_text_color(text_seconds_layer, GColorWhite);
    text_layer_set_background_color(text_seconds_layer, GColorClear);
    text_layer_set_text_alignment(text_seconds_layer, GTextAlignmentCenter);
    text_layer_set_font(text_seconds_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_KLINGON_25)));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_seconds_layer));

    tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
}

static void deinit(void) {
    tick_timer_service_unsubscribe();
    text_layer_destroy(text_seconds_layer);
    text_layer_destroy(text_minutes_layer);
    text_layer_destroy(text_hours_layer);
    gbitmap_destroy(background_bitmap);
    bitmap_layer_destroy(background_layer);
    window_destroy(window);
}

int main(void) {
    init();
    app_event_loop();
    deinit();

    return 0;
}
