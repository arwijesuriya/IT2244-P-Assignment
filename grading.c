#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

void calculate_grade(int mark, char *grade) {
    if (mark > 74) {
        *grade = 'A';
    } 
    else if (mark > 60) {
        *grade = 'B';
    } 
    else if (mark > 45) {
        *grade = 'C';
    } 
    else if (mark > 34) {
        *grade = 'D';
    } 
    else {
        *grade = 'E';
    }
}

int main() {
    int marks[3];
    int pipes[3][2]; // Pipes for sending marks from parent to children
    int result_pipes[3][2]; // Pipes for sending grades from children to parent
    pid_t pids[3];

    // Create pipes
    for (int i = 0; i < 3; i++) {
        if (pipe(pipes[i]) == -1 || pipe(result_pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Fork child processes
    for (int i = 0; i < 3; i++) {
        if ((pids[i] = fork()) == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) { // Child process
            close(pipes[i][1]); // Close write end of pipe to read mark from parent
            close(result_pipes[i][0]); // Close read end of pipe to send grade to parent

            int mark;
            read(pipes[i][0], &mark, sizeof(mark));

            char grade;
            calculate_grade(mark, &grade);

            write(result_pipes[i][1], &grade, sizeof(grade));


            close(pipes[i][0]);
            close(result_pipes[i][1]);
            exit(0);
        }
    }

    // Parent process
    close(pipes[0][0]);
    close(pipes[1][0]);
    close(pipes[2][0]);
    close(result_pipes[0][1]);
    close(result_pipes[1][1]);
    close(result_pipes[2][1]);

    // Read marks from user
    printf("Enter marks for three subjects:\n");
    for (int i = 0; i < 3; i++) {
        printf("Subject %d: ", i + 1);
        scanf("%d", &marks[i]);
        write(pipes[i][1], &marks[i], sizeof(marks[i]));
    }

    char grades[3];
    for (int i = 0; i < 3; i++) {
        read(result_pipes[i][0], &grades[i], sizeof(grades[i]));
    }

    // Print result
    printf("Subject\tMark\tGrade\n");
    for (int i = 0; i < 3; i++) {
        printf("Subject %d\t%d\t%c\n", i + 1, marks[i], grades[i]);
    }

    // Print parent process ID
    printf("Parent Process ID: %d\n", getpid());

    // Close remaining pipe ends
    close(pipes[0][1]);
    close(pipes[1][1]);
    close(pipes[2][1]);
    close(result_pipes[0][0]);
    close(result_pipes[1][0]);
    close(result_pipes[2][0]);

    // Wait for child processes to finish
    for (int i = 0; i < 3; i++) {
        wait(NULL);
    }

    return 0;
}