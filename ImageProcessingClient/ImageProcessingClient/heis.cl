__kernel void  test(__global int* message)
{
    // Получаем текущий id
    int gid = get_global_id(0);

    message[gid] += gid;
}