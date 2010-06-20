int main() {
  int a[4];
  int i;
  int q;
  int r;
  int s=1839;
  int t;
  int x;
  int y;
  int z[1920 * sizeof(int)];

  for (x = 0; x < 1920 * sizeof(int); x++) {
    z[x] = 0;
  }

  z[1000]=1;
  z[1001]=1;
  z[1080]=2;
  z[1081]=2;

f:
  putchar('\n');
  for (i = 0; i < 1920; i++) {
    putchar(z[i] + 32);
    a[0] = i % 80 > 0 ? i - 1 : i + 79;
    a[1] = i % 80 < 79 ? i + 1: i - 79;
    a[2] = i > 79 ? i - 80 : s + i + 1;
    a[3] = i < s ? i + 80 : i - s - 1;
    x = random() % 4;
    y = random() % 16;
    q = z[a[x]];
    r = z[i];
    if (q) {
      if (r == q) {
        z[a[0]] = (r + y + q) % 16;
      } else if (r > q) {
        t = (r * q) % 16;
        z[a[x]] = r;
        z[i]= t;
      }
    } else {
      z[a[x]] = r;
      z[i] = 0;
    }
  }
  usleep(48000);

  goto f;
}
