https://www.shadertoy.com/

# Ray Tracing in One Weekend

```glsl
#define FLT_MAX 3.402823466e+38

// 窗口大小
const vec2 windowSize = vec2(640, 360);
// 相机中心
const vec2 center = windowSize/2.0;
const int sample_count = 5;
const int total_count = 25;
const float sample_interval = 1.0 / float(sample_count + 2);
const int loop_count = 10;
const int sphere_count = 4;
vec2 frag_coord;

// 定义 Material 类型
const int material_type_lambertian = 0;
const int material_type_metal = 1;

struct ray {
    vec3 origin;
    vec3 direction;
};

struct material {
    vec3 color;
    int type;
    // Optional
    float fuzz;
};

struct sphere {
    vec3 center;
    material mat;
    float radius;
};

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material mat;
    bool front_face;
};

void set_face_normal(inout hit_record hit, in ray r, in vec3 outward_normal) {
    hit.front_face = dot(r.direction, outward_normal) < 0.0;
    if (hit.front_face) {
        hit.normal = outward_normal;
    } else {
        hit.normal = -outward_normal;
    }
}

vec3 point_at_parameter(float t, in ray r) {
    return r.origin + t * r.direction;
}

// 用来生成一点随机数
float hash(float n)
{
    return fract (sin(n)*43758.5453);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec3 random_in_unit_sphere(in ray r) {
    vec3 p = vec3(0.0);
    float index = 0.0;
    do {
        float random_num1 = hash(hash(index));
        float random_num2 = random(vec2(r.direction.x, r.direction.y));
        float random_num = random_num1 + random_num2;
        p = 2.0*vec3(hash(random_num), hash(random_num+100.0), hash(random_num+200.0))-vec3(1.0);
        index += 1.0;
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 random_in_hemisphere(in ray r, in vec3 normal) {
    vec3 in_unit_sphere = random_in_unit_sphere(r);
    if (dot(in_unit_sphere, normal) > 0.0)
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// 金属材质反射
bool metal_scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    vec3 reflected = reflect(normalize(r.direction), rec.normal);
    if (mat.fuzz > 1.0) mat.fuzz = 1.0;
    scattered = ray(rec.p, reflected + mat.fuzz*random_in_unit_sphere(r));
    attenuation = mat.color;
    return dot(scattered.direction, rec.normal) > 0.0;
}

// 漫反射材质反射
bool lambertian_scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    vec3 scatter_direction = rec.normal + normalize(random_in_unit_sphere(r));
    scattered = ray(rec.p, scatter_direction);
    attenuation = mat.color;
    return true;
}

// 反射逻辑
bool scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    switch(mat.type) {
    case 0:
        return metal_scatter(mat, r, rec, attenuation, scattered);
    case 1:
        return lambertian_scatter(mat, r, rec, attenuation, scattered);
    }
}

// 击中球体
bool hit_sphere(in sphere s, in ray r, float t_min, float t_max, inout hit_record rec) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float half_b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius * s.radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0.0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lines in the acceptable range.
    float root = (-half_b - sqrtd)/a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) return false;
    }

    rec.t = root;
    rec.p = point_at_parameter(root, r);
    vec3 outward_normal = (rec.p - s.center) / s.radius;
    set_face_normal(rec, r, outward_normal);
    rec.mat = s.mat;
    return true;
}

bool hit_list(in ray r, in sphere s[sphere_count], float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (int i = 0; i < sphere_count; ++i) {
        if (hit_sphere(s[i], r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

// 生成场景球体的方法
sphere[sphere_count] create_sphere() {
    sphere s1 = sphere(vec3( 0.0, -100.5, -1.0), material(vec3(0.8, 0.8, 0.0), 1, 0.0), 100.0);
    sphere s2 = sphere(vec3( 0.0,    0.0, -1.0), material(vec3(0.7, 0.3, 0.3), 1, 0.0),   0.5);
    sphere s3 = sphere(vec3(-1.0,    0.0, -1.0), material(vec3(0.8, 0.8, 0.8), 0, 0.0),   0.5);
    sphere s4 = sphere(vec3( 1.0,    0.0, -1.0), material(vec3(0.8, 0.6, 0.2), 0, 0.0),   0.5);
    return sphere[](s1, s2, s3, s4);
}

vec3 color(in ray r) {
    hit_record rec;

    //sphere s1 = sphere(vec3(0.0, 0.0, -1.0), 0.5);
    //sphere s2 = sphere(vec3(0.0, -100.5, -1.0), 100.0);

    //sphere s_list[2] = sphere[](s1, s2);
    sphere s_list[sphere_count] = create_sphere();

    int max_depth = 50;

    // float result_value = 1.0;

    ray temp_ray = r;
    ray scattered = r;
    vec3 attenuation;
    vec3 result_attenuation = vec3(1.0);
    int index;

    vec3 result;

    for (index = 0; index < loop_count; ++index) {
        if (!hit_list(temp_ray, s_list, 0.001, FLT_MAX, rec)) {
            vec3 unit_direction = normalize(temp_ray.direction);
            float t = 0.5 * (unit_direction.y + 1.0);
            result = ((1.0 - t) * vec3(1.0) + t * vec3(0.5, 0.7, 1.0));
            break;
        }
        if (!scatter(rec.mat, temp_ray, rec, attenuation, scattered)) {
            result = vec3(0.0);
            break;
        }
        temp_ray = scattered;
        result_attenuation *= attenuation;

    }
    return result * result_attenuation;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    frag_coord = fragCoord;

    // 抗锯齿
    ray r_list[total_count];
    for (int i = 0; i < sample_count; ++i) {
        float y = float(i+1)*sample_interval-0.5;
        for (int j = 0; j < sample_count; ++j) {
            float x = float(j+1)*sample_interval-0.5;
            r_list[i*sample_count+j] = ray(vec3(0.0), vec3(fragCoord.x+x, fragCoord.y+y, 0.0)-vec3(center.xyy));
        }
    }

    vec3 pixel_color = vec3(0.0);

    for (int i = 0; i < total_count; ++ i) {
        pixel_color += color(r_list[i]);
    }


    fragColor = vec4(sqrt(pixel_color.r / float(total_count)), sqrt(pixel_color.g / float(total_count)), sqrt(pixel_color.b / float(total_count)), 1.0);
}
```
# 临时记录
```glsl
#define FLT_MAX 3.402823466e+38

vec2 frag_coord;

struct ray {
    vec3 origin;
    vec3 direction;
};

struct sphere {
    vec3 center;
    float radius;
};

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    bool front_face;
};

void set_face_normal(inout hit_record hit, inout ray r, inout vec3 outward_normal) {
    hit.front_face = dot(r.direction, outward_normal) < 0.0;
    if (hit.front_face) {
        hit.normal = outward_normal;
    } else {
        hit.normal = -outward_normal;
    }
}

vec3 point_at_parameter(float t, ray r) {
    return r.origin + t * r.direction;
}

// 用来生成一点随机数
float hash( float n )
{
    return fract (sin(n)*43758.5453);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*
        43758.5453123);
}

vec3 random_in_unit_sphere() {
    vec3 p = vec3(0.0);
    float index = 0.0;
    do {
        float random_num1 = hash(hash(index));
        float random_num2 = random(frag_coord);
        float random_num = random_num1 + random_num2;
        p = 2.0*vec3(hash(random_num), hash(random_num+100.0), hash(random_num+200.0))-vec3(1.0);
        index += 1.0;
    } while (dot(p, p) >= 1.0);
    return p;
}

// 击中球体
bool hit_sphere(sphere s, ray r, float t_min, float t_max, inout hit_record rec) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float half_b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius * s.radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0.0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lines in the acceptable range.
    float root = (-half_b - sqrtd)/a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) return false;
    }

    rec.t = root;
    rec.p = point_at_parameter(root, r);
    vec3 outward_normal = (rec.p - s.center) / s.radius;
    set_face_normal(rec, r, outward_normal);
    return true;
}

bool hit_list(ray r, sphere s[2], float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec = hit_record(0.0, vec3(0.0), vec3(0.0), false);
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (int i = 0; i < 2; ++i) {
        if (hit_sphere(s[i], r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

vec3 color(ray r) {
    hit_record rec;

    sphere s1 = sphere(vec3(0.0, 0.0, -1.0), 0.5);
    sphere s2 = sphere(vec3(0.0, -100.5, -1.0), 100.0);

    sphere s_list[2] = sphere[](s1, s2);

    int max_depth = 50;

    ray temp_ray = r;
    float result_value = 1.0;

    for (int i = 0; i < 50; ++i) {
        if (!hit_list(temp_ray, s_list, 0.0, FLT_MAX, rec)) {
            vec3 unit_direction = normalize(temp_ray.direction);
            float t = 0.5 * (unit_direction.y + 1.0);
            return (1.0 - t) * vec3(1.0) + t * vec3(0.3, 0.7, 1.0) * result_value;
        }
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        temp_ray = ray(rec.p, target-rec.p);
        result_value *= 0.5;
    }
    return vec3(0.0);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    frag_coord = fragCoord;

    // 抗锯齿
    ray r_list[4];
    r_list[0] = ray(vec3(0.0), vec3(fragCoord.x-320.25, fragCoord.y-180.00, -100.0));
    r_list[1] = ray(vec3(0.0), vec3(fragCoord.x-320.00, fragCoord.y-180.25, -100.0));
    r_list[2] = ray(vec3(0.0), vec3(fragCoord.x-319.75, fragCoord.y-180.00, -100.0));
    r_list[3] = ray(vec3(0.0), vec3(fragCoord.x-320.00, fragCoord.y-179.75, -100.0));

    vec3 pixel_color = vec3(0.0);

    for (int i = 0; i < 4; ++ i) {
        pixel_color += color(r_list[i]);
    }


    fragColor = vec4(pixel_color / 4.0, 1.0);

    //ray r = ray(vec3(0.0), vec3(fragCoord.x-320.0, fragCoord.y-180.0, 100.0));
    //fragColor = vec4(color(r), 1.0);
}
```

#define FLT_MAX 3.402823466e+38

// 窗口大小
const vec2 windowSize = vec2(640, 360);
// 相机中心
const vec2 center = windowSize/2.0;
const int sample_count = 5;
const int total_count = 25;
const float sample_interval = 1.0 / float(sample_count + 2);
const int loop_count = 10;
const int sphere_count = 4;
vec2 frag_coord;

// 定义 Material 类型
const int material_type_lambertian = 0;
const int material_type_metal = 1;

struct ray {
    vec3 origin;
    vec3 direction;
};

struct material {
    vec3 color;
    int type;
};

struct sphere {
    vec3 center;
    material mat;
    float radius;
};

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material mat;
    bool front_face;
};

void set_face_normal(inout hit_record hit, in ray r, in vec3 outward_normal) {
    hit.front_face = dot(r.direction, outward_normal) < 0.0;
    if (hit.front_face) {
        hit.normal = outward_normal;
    } else {
        hit.normal = -outward_normal;
    }
}

vec3 point_at_parameter(float t, in ray r) {
    return r.origin + t * r.direction;
}

// 用来生成一点随机数
float hash(float n)
{
    return fract (sin(n)*43758.5453);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy, vec2(12.9898,78.233)))*43758.5453123);
}

vec3 random_in_unit_sphere(in ray r) {
    vec3 p = vec3(0.0);
    float index = 0.0;
    do {
        float random_num1 = hash(hash(index));
        float random_num2 = random(vec2(r.direction.x, r.direction.y));
        float random_num = random_num1 + random_num2;
        p = 2.0*vec3(hash(random_num), hash(random_num+100.0), hash(random_num+200.0))-vec3(1.0);
        index += 1.0;
    } while (dot(p, p) >= 1.0);
    return p;
}

vec3 random_in_hemisphere(in ray r, in vec3 normal) {
    vec3 in_unit_sphere = random_in_unit_sphere(r);
    if (dot(in_unit_sphere, normal) > 0.0)
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// 金属材质反射
bool metal_scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    vec3 reflected = reflect(normalize(r.direction), rec.normal);
    scattered = ray(rec.p, reflected);
    attenuation = mat.color;
    return dot(scattered.direction, rec.normal) > 0.0;
}

// 漫反射材质反射
bool lambertian_scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    vec3 scatter_direction = rec.normal + normalize(random_in_unit_sphere(r));
    scattered = ray(rec.p, scatter_direction);
    attenuation = mat.color;
    return true;
}

bool scatter(in material mat, in ray r, inout hit_record rec, inout vec3 attenuation, inout ray scattered) {
    switch(mat.type) {
    case 0:
        return metal_scatter(mat, r, rec, attenuation, scattered);
    case 1:
        return lambertian_scatter(mat, r, rec, attenuation, scattered);
    }
}

// 击中球体
bool hit_sphere(in sphere s, in ray r, float t_min, float t_max, inout hit_record rec) {
    vec3 oc = r.origin - s.center;
    float a = dot(r.direction, r.direction);
    float half_b = dot(oc, r.direction);
    float c = dot(oc, oc) - s.radius * s.radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0.0) return false;
    float sqrtd = sqrt(discriminant);

    // Find the nearest root that lines in the acceptable range.
    float root = (-half_b - sqrtd)/a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) return false;
    }

    rec.t = root;
    rec.p = point_at_parameter(root, r);
    vec3 outward_normal = (rec.p - s.center) / s.radius;
    set_face_normal(rec, r, outward_normal);
    rec.mat = s.mat;
    return true;
}

bool hit_list(in ray r, in sphere s[sphere_count], float t_min, float t_max, inout hit_record rec) {
    hit_record temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (int i = 0; i < sphere_count; ++i) {
        if (hit_sphere(s[i], r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    return hit_anything;
}

// 生成场景球体的方法
sphere[sphere_count] create_sphere() {
    sphere s1 = sphere(vec3( 0.0, -100.5, -1.0), material(vec3(0.8, 0.8, 0.0), 1), 100.0);
    sphere s2 = sphere(vec3( 0.0,    0.0, -1.0), material(vec3(0.7, 0.3, 0.3), 1),   0.5);
    sphere s3 = sphere(vec3(-1.0,    0.0, -1.0), material(vec3(0.8, 0.8, 0.0), 1),   0.5);
    sphere s4 = sphere(vec3( 1.0,    0.0, -1.0), material(vec3(0.8, 0.8, 0.0), 1),   0.5);
    return sphere[](s1, s2, s3, s4);
}

vec3 color(in ray r) {
    hit_record rec;

    //sphere s1 = sphere(vec3(0.0, 0.0, -1.0), 0.5);
    //sphere s2 = sphere(vec3(0.0, -100.5, -1.0), 100.0);

    //sphere s_list[2] = sphere[](s1, s2);
    sphere s_list[sphere_count] = create_sphere();

    int max_depth = 50;

    // float result_value = 1.0;

    ray temp_ray = r;
    ray scattered = r;
    vec3 attenuation;
    vec3 result_attenuation = vec3(1.0);
    int index;

    vec3 result;

    for (index = 0; index < loop_count; ++index) {
        if (!hit_list(temp_ray, s_list, 0.001, FLT_MAX, rec)) {
            vec3 unit_direction = normalize(temp_ray.direction);
            float t = 0.5 * (unit_direction.y + 1.0);
            result = ((1.0 - t) * vec3(1.0) + t * vec3(0.3, 0.7, 1.0));
        }
        if (!scatter(rec.mat, temp_ray, rec, attenuation, scattered)) {
            result = vec3(0.0);
        }
        temp_ray = scattered;
        result_attenuation *= attenuation;
        //vec3 target = rec.p + random_in_hemisphere(r, rec.normal);
        // vec3 target = rec.p + rec.normal + normalize(random_in_unit_sphere(r));
        //temp_ray = ray(rec.p, target-rec.p);
        //result_value *= 0.5;

    }
    return result * result_attenuation;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
    frag_coord = fragCoord;

    // 抗锯齿
    ray r_list[total_count];
    for (int i = 0; i < sample_count; ++i) {
        float y = float(i+1)*sample_interval-0.5;
        for (int j = 0; j < sample_count; ++j) {
            float x = float(j+1)*sample_interval-0.5;
            r_list[i*sample_count+j] = ray(vec3(0.0), vec3(fragCoord.x+x, fragCoord.y+y, 0.0)-vec3(center.xyy));
        }
    }

    vec3 pixel_color = vec3(0.0);

    for (int i = 0; i < total_count; ++ i) {
        pixel_color += color(r_list[i]);
    }


    fragColor = vec4(sqrt(pixel_color.r / float(total_count)), sqrt(pixel_color.g / float(total_count)), sqrt(pixel_color.b / float(total_count)), 1.0);
}
