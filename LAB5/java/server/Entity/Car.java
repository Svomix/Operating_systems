package lab5.Entity;

import jakarta.persistence.*;
import lombok.AllArgsConstructor;
import lombok.Builder;
import lombok.Data;
import lombok.NoArgsConstructor;

@Entity
@Data
@Builder
@NoArgsConstructor
@AllArgsConstructor
@Table(name = "cars")
public class Car {
    @Id
    @GeneratedValue(
            strategy = GenerationType.SEQUENCE,
            generator = "seq_gen"
    )
    @SequenceGenerator(
            name = "seq_gen",
            sequenceName = "cars_seq",
            allocationSize = 1
    )
    Long id;
    @Column(name = "max_speed")
    int maxSpeed;
    @Column(name = "tank_capacity")
    int tankCapacity;
    @Column(name = "engine_power")
    int enginePower;
    int price;
    String name;
}
