// comments
@import "std";

enum HairColor {
    Brown,
    Black,
    Blonde,
    Red,
}

struct Person {
    let name: String,
    let age: Int,
    let hair_color: HairColor,
}

method Person {
    fn new(name: String, age: Int) Person {
        return Person{
            .name: name,
            .age: age,
            .hair_color: HairColor::Brown,
        }.
    }

    fn get_name(self) String {
        return self.name.
    }
}


fn higher_or_lower(a: int32, b: Int) String {
    answer := "".
    if (a > b) {
        answer = "higher".
    } else {
        answer = "lower".
    }
    return answer.
}

void main() {
    person := Person::new(.name: "John", .age: 30, .hair_color: HairColor::Red).
    std::print(person.name).
    std::print(person.age).

    x := 10 - 5.

    if (x > 0) ? std::print("x is positive") : std::print("x is negative").

    answer := higher_or_lower(10, 20).
    std::print(answer).
}
