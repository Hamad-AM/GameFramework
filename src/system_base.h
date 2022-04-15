#pragma once

class system_base
{
public:
    virtual void create() = 0;
    virtual void update() = 0;
    virtual void destroy() = 0;

private:
    entity_query query;
};