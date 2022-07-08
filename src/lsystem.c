#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define BUFFER_SIZE 1024 * 1024
#define FIELD_SIZE 0xFF

typedef struct rule {
    char in;
    char out[FIELD_SIZE];
} Rule;


typedef struct lsystem {
    char constants[FIELD_SIZE];
    char axiom[FIELD_SIZE];
    Rule rules[FIELD_SIZE];
    double angle;
} Lsystem;



char *lookup_rule(Lsystem *grammar, char in) {
    for (size_t i = 0; grammar->rules[i].in != (char)0; ++i) {
        if (in == grammar->rules[i].in) {
            return grammar->rules[i].out;
        }
    }
    return "";
}
int contains_constant(Lsystem *grammar, char possible_constant) {
    for (int i = 0; i < FIELD_SIZE; ++i) {
        if (grammar->constants[i] == possible_constant) return 1;
    }
    return 0;
}

char *generate_lsystem(Lsystem *grammar, size_t num_iter, char *in_buffer, char *out_buffer, size_t buffer_size) {
    memset(in_buffer, '\0',  buffer_size);
    memset(out_buffer, '\0', buffer_size);

    strncpy(in_buffer, grammar->axiom, strlen(grammar->axiom));


    for (int i = 0; i < num_iter; ++i) {
        printf("%d :: %s\n", i + 1, in_buffer);
        for (size_t index = 0; index < buffer_size; ++index) {
            // printf("\t%ld ---- ", index);

            // append rule lookup

            if (contains_constant(grammar, in_buffer[index])) {
                strncat(out_buffer, &in_buffer[index], 1);
            }
            else {
                char *to_write = lookup_rule(grammar, in_buffer[index]);
                strncat(out_buffer, to_write, strlen(to_write));
            }

            // printf("\t%s\n", out_buffer);
        }

        // move out to in if not the last iteration
        if (i < num_iter - 1) {
            strncpy(in_buffer, out_buffer, strlen(out_buffer));
            memset(out_buffer, '\0', strlen(out_buffer));
        }
    }
    return out_buffer;
}

int generate_lines(double *lines, char *instructions, double angle_delta, size_t instruction_length) {
    double forward_distance = 20;

    double saved_position_stack[512][3];
    size_t stack_size = 0;

    // current values
    double x = 0;
    double y = 0;
	double angle = 0;

    int buffer_size = -1;
    // printf("%s %ld\n", instructions, instruction_length);

    for (int i = 0; i < instruction_length; ++i) {
        // printf("%c ", instructions[i]);
        switch(instructions[i]) {
            // move forward
            case 'F': {
                if (buffer_size == -1) buffer_size++;

                // extend (x, y) with (0, forward_distance) and rotate to by the angle
                double new_x = x + forward_distance*sin(angle);
                double new_y = y - forward_distance*cos(angle);

                lines[buffer_size + 0] = +x;
                lines[buffer_size + 1] = -y;
                lines[buffer_size + 2] = +new_x;
                lines[buffer_size + 3] = -new_y;
                buffer_size += 4;

                // printf("%f, %f -> %f %f\n", x, y, new_x, new_y);

                y = new_y;
                x = new_x;
                break;
            }
            // update angle
            case '-':
                angle = fmod(angle - angle_delta, 2*M_PI);
                // printf("%f \n", angle / M_PI * 180);
                break;
            case '+':
                angle = fmod(angle + angle_delta, 2*M_PI);
                // printf("%f \n", angle / M_PI * 180);
                break;
            // push/pop position and angle stack
            case '[': {
                // saved_position.push(tuple<double, double, double>(x, y, angle));
                saved_position_stack[stack_size][0] = x;
                saved_position_stack[stack_size][1] = y;
                saved_position_stack[stack_size][2] = angle;
                stack_size++;
                // printf("\t");
                // printf("push (%ld) %f, %f (%f)\n", stack_size, x, y, angle);
                break;

            }
            case ']': {
                stack_size--;
				x = saved_position_stack[stack_size][0];
				y = saved_position_stack[stack_size][1];
				angle = saved_position_stack[stack_size][2];
                // printf("\t\t");
                // printf("pop (%ld) %f, %f (%f)\n", stack_size, x, y, angle);
                break;
            }
            default: 
                // printf("\n");
                break;
        }
    }
    return buffer_size;
}

double *create_lsystem(size_t num_iter, int *size_ptr) {
    Lsystem lsystem = {
        // hilbert
        {'+', '-', '[', ']', 'F'},
        "X",
        {{'X', "-YF+XFX+FY-"}, {'Y', "+XF-YFY-FX+"}},
        M_PI/2
    };
    

    char *in_buffer  = calloc(sizeof(char), BUFFER_SIZE);
    char *out_buffer = calloc(sizeof(char), BUFFER_SIZE);
    double *lines = calloc(sizeof(double),  BUFFER_SIZE);


    char *instructions = generate_lsystem(&lsystem, num_iter, in_buffer, out_buffer, BUFFER_SIZE);
    // printf("%s (%ld)\n", instructions, strlen(instructions));

    int line_length = generate_lines(lines, instructions, lsystem.angle, strlen(instructions));
    *size_ptr = line_length;

    // printf("%f, %f -> %f %f\n", x, y, new_x, new_y);
    free(in_buffer);
    free(out_buffer);

    return lines;
}

// int main(int argc, char* argv[])
// {
//     int line_length = 0;
//     double *lines = create_lsystem(5, &line_length);

//     // printf("%d\n", line_length);
//     for (int i = 0; i < line_length - 2; i+=2) {
//         printf("%f %f -> %f %f\n", lines[i + 0], lines[i + 1], lines[i + 2], lines[i + 3]);
//     }

//     free(lines);

//     return 0;
// }
