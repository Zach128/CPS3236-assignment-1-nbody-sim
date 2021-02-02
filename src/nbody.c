#include <math.h>

#include "vector2.h"
#include "bpoint.h"

void ComputeForces(b_point *bodies, int body_count, float grav_constant, float time_delta)
{
	vec2 direction,
		force, acceleration, buffer;

	float distance;

	for (int j = 0; j < body_count; ++j)
	{
		b_point *p1 = &bodies[j];
	
		force.x = acceleration.x = buffer.x = 0.f;
        force.y = acceleration.y = buffer.y = 0.f;
	
		for (int k = 0; k < body_count; ++k)
		{
			if (k == j) continue;
		
			b_point *p2 = &bodies[k];
			
			// Compute direction vector.
			direction.x = p2->pos.x - p1->pos.x;
            direction.y = p2->pos.y - p1->pos.y;

            // Calculate the length of the computed direction.
            double direction_length = vec2Length(&direction);
			
			// Limit distance term to avoid singularities.
			// distance = std::max<float>( 0.5f * (p2.Mass + p1.Mass), fabs(direction.Length()) );
            distance = 0.5f * (p2->mass + p1->mass);
            distance = distance >= direction_length ? distance : direction_length;
			
			// Accumulate force. Follows the below calculation
			// force += direction / (distance * distance * distance) * p2->mass; 
			vec2Divf(&direction, (distance * distance * distance), &force);
			vec2Mulf(&force, p2->mass, &force);
		}
				
		// Compute acceleration for body.
		// acceleration = force * p_gravitationalTerm;
		vec2Mulf(&force, grav_constant, &acceleration);

		// Integrate velocity (m/s).
		// p1.Velocity += acceleration * p_deltaT;
		vec2Addvec2(&p1->vel, vec2Mulf(&acceleration, time_delta, &buffer), &p1->vel);
	}
}

void MoveBodies(b_point *bodies, int body_count, float time_delta)
{
	vec2 buffer = { 0.f, 0.f };
	for (int i = 0; i < body_count; i++)
	{
		// bodies[i]->pos += bodies[i].vel * time_delta;
		vec2Mulf(&bodies[i].vel, time_delta, &buffer);
		vec2Addvec2(&bodies[i].pos, &buffer, &bodies[i].pos);
	}
}
