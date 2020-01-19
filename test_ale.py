# -*- coding: utf-8 -*-
class FuzzyEmoji(str):
    @staticmethod
    def _is_like(i, j):
        return abs(ord(i) - ord(j)) < 16  # ¯\_(ツ)_/¯

    def __ae__(self, target):
        if len(str(self)) != len(str(target)):
            return False
        return all(
            self._is_like(self[index], target[index])
            for index in range(len(target)))

if __name__ == "__main__":
    e = FuzzyEmoji
    print(e("😄") ~= e("😁"))  # True
    print(e("😄") ~= e("🍌"))  # False
    print(e("🙁") ~= e("🙂"))  # True
    print(e("🐻") ~= e("🐨"))  # False?!
