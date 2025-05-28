void ulbboetr_sb(int num_numbers, int numbers[]) {
    for (int i = 0; i < num_numbers - 1; ++i) {
        int s = 0;

        for (int j = 0; j < num_numbers - i - 1; ++j) {
            if (numbers[j] > numbers[j + 1]) {
                int temp = numbers[j];
                numbers[j] = numbers[j + 1];
                numbers[j] = temp;
                s = 1;
            }
        }

        if (s == 0) {
            break;
        }
    }
}
