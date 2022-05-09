const float peak = 9.5;
const float trough = 8.5;

void step_reporter_fsm(float avg_acc_mag)
{
    switch (step_counter_state)
    {
    case REST:
        if (avg_acc_mag >= peak)
        {
            step_counter_state = WALK_UP;
        }
        break;

    case WALK_UP:
        if (avg_acc_mag <= trough)
        {
            step_counter_state = WALK_DOWN;
            step_timer = millis();
        }
        break;

    case WALK_DOWN:
        if (millis() - step_timer > 5000)
        {
            step_counter_state = REST;
        }
        else if (avg_acc_mag >= peak && millis() - step_timer > 200)
        {
            step_counter_state = WALK_UP;
            steps++;
            new_step = true;
            posting_timer = millis();
        }
        break;
    }
}

void update_step_count()
{

    sensors_event_t accel_event;
    accel.getEvent(&accel_event);

    float x = accel_event.acceleration.x;
    float y = accel_event.acceleration.y;
    float z = accel_event.acceleration.z;

    float acc_mag = sqrt(x * x + y * y + z * z);
    float avg_acc_mag = 1.0 / 3.0 * (acc_mag + old_acc_mag + older_acc_mag);
    older_acc_mag = old_acc_mag;
    old_acc_mag = acc_mag;

    step_reporter_fsm(avg_acc_mag);

    Serial.printf("Updated step reporter fsm, current step %d, current accel %f, %f, %f\n", steps, x, y, z);

    if (WiFi.status() == WL_CONNECTED && new_step && millis() - posting_timer > WAITING_PERIOD)
    {
        char step_body[100];                                    // for step_body
        sprintf(step_body, "username=%s&step=%d", USER, steps); // generate step_body, posting to User, 1 step
        int body_len = strlen(step_body);                       // calculate step_body length (for header reporting)
        sprintf(request_buffer, "POST http://608dev-2.net/sandbox/sc/team42/608_team42_final/create_step_db.py HTTP/1.1\r\n");
        strcat(request_buffer, "Host: 608dev-2.net\r\n");
        strcat(request_buffer, "Content-Type: application/x-www-form-urlencoded\r\n");
        sprintf(request_buffer + strlen(request_buffer), "Content-Length: %d\r\n", body_len); // append string formatted to end of request buffer
        strcat(request_buffer, "\r\n");                                                       // new line from header to step_body
        strcat(request_buffer, step_body);                                                    // step_body
        strcat(request_buffer, "\r\n");                                                       // new line
        do_http_request(TEAM_SERVER, request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
        // tft.fillScreen(TFT_BLACK); // black out TFT Screen
        steps = 0;
        new_step = false;
    }
}