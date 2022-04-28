void createNeedle(void)
{
    needle.setColorDepth(16);
    needle.createSprite(NEEDLE_WIDTH, NEEDLE_LENGTH); // create the needle Sprite

    needle.fillSprite(TFT_BLACK); // Fill with black

    // Define needle pivot point relative to top left corner of Sprite
    uint16_t piv_x = NEEDLE_WIDTH / 2; // pivot x in Sprite (middle)
    uint16_t piv_y = NEEDLE_RADIUS;    // pivot y in Sprite
    needle.setPivot(piv_x, piv_y);     // Set pivot point in this Sprite

    // Draw the red needle in the Sprite
    needle.fillRect(0, 0, NEEDLE_WIDTH, NEEDLE_LENGTH, TFT_MAROON);
    needle.fillRect(1, 1, NEEDLE_WIDTH - 2, NEEDLE_LENGTH - 2, TFT_RED);

    // Bounding box parameters to be populated
    int16_t min_x;
    int16_t min_y;
    int16_t max_x;
    int16_t max_y;

    // Work out the worst case area that must be grabbed from the TFT,
    // this is at a 45 degree rotation
    needle.getRotatedBounds(45, &min_x, &min_y, &max_x, &max_y);

    // Calculate the size and allocate the buffer for the grabbed TFT area
    tft_buffer = (uint16_t *)malloc(((max_x - min_x) + 2) * ((max_y - min_y) + 2) * 2);
}

void plotNeedle(int16_t angle, uint16_t ms_delay)
{
    
    // Serial.println(angle);

    static int16_t old_angle = -180; // Starts at -180 degrees

    // Bounding box parameters
    static int16_t min_x;
    static int16_t min_y;
    static int16_t max_x;
    static int16_t max_y;

    angle -= 180; // Starts at -180 degrees

    // Move the needle until new angle reached
    while (angle != old_angle || !buffer_loaded)
    {

        if (old_angle < angle)
            old_angle++;
        else
            old_angle--;

        // Only plot needle at even values to improve plotting performance
        if ((old_angle & 1) == 0)
        {
            if (buffer_loaded)
            {
                // Paste back the original needle free image area
                tft.pushRect(min_x, min_y, 1 + max_x - min_x, 1 + max_y - min_y, tft_buffer);
            }

            if (needle.getRotatedBounds(old_angle, &min_x, &min_y, &max_x, &max_y))
            {
                // Grab a copy of the area before needle is drawn
                tft.readRect(min_x, min_y, 1 + max_x - min_x, 1 + max_y - min_y, tft_buffer);
                buffer_loaded = true;
            }

            // Draw the needle in the new position, black in needle image is transparent
            needle.pushRotated(old_angle, TFT_BLACK);

            // Wait before next update
            delay(ms_delay);
        }

        // Update the number at the centre of the dial

        // Slow needle down slightly as it approaches the new position
        if (abs(old_angle - angle) < 10)
            ms_delay += ms_delay / 5;
    }
}

void setup_compass()
{

    tft.setPivot(SCREEN_CENTRE_X, SCREEN_CENTRE_Y);

    // Create the needle Sprite
    createNeedle();

    // Reset needle position to 0
    //   plotNeedle(0, 0);

    /* Initialise the sensor */
    if (!mag.begin())
    {
        /* There was a problem detecting the LSM303 ... check your connections */
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while (1)
            ;
    }

    if (!accel.begin())
    {
        Serial.println("Ooops, no LSM303 detected ... Check your wiring!");
        while (1)
            ;
    }

    accel.setRange(LSM303_RANGE_4G);
    Serial.print("Range set to: ");
    lsm303_accel_range_t new_range = accel.getRange();
    switch (new_range)
    {
    case LSM303_RANGE_2G:
        Serial.println("+- 2G");
        break;
    case LSM303_RANGE_4G:
        Serial.println("+- 4G");
        break;
    case LSM303_RANGE_8G:
        Serial.println("+- 8G");
        break;
    case LSM303_RANGE_16G:
        Serial.println("+- 16G");
        break;
    }

    accel.setMode(LSM303_MODE_NORMAL);
    Serial.print("Mode set to: ");
    lsm303_accel_mode_t new_mode = accel.getMode();
    switch (new_mode)
    {
    case LSM303_MODE_NORMAL:
        Serial.println("Normal");
        break;
    case LSM303_MODE_LOW_POWER:
        Serial.println("Low Power");
        break;
    case LSM303_MODE_HIGH_RESOLUTION:
        Serial.println("High Resolution");
        break;
    }
}