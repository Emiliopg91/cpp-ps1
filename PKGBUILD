pkgname=cpp-ps1
pkgver=1.0.0
pkgrel=1
pkgdesc='C++ base PS1 function, useful for programmers'
arch=('x86_64')
url='https://github.com/Emiliopg91/cpp-ps1'
license=('GPL-2')
depends=(
  'git'
  'libgit2'
  'clang'
)
source=(
  "git+$url.git"
)
sha256sums=(
  'SKIP'
)
install=${pkgname}.install

build() {
    cd "$srcdir/cpp-ps1"
    clang++ -O3 -lgit2 -ffast-math -march=native -flto -fno-exceptions -fno-rtti -pipe -s -std=c++17 cpp-ps1.cpp -o $DIR/cpp-ps1
}

package() {
  cd "$srcdir/cpp-ps1"

  install -Dm755 cpp-ps1 "$pkgdir/usr/bin/cpp-ps1"
}